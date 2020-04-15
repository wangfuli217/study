#include "net/net.hh"
#include "msg_helper.hh"
#include "examples.hh"

namespace echo_server_sel {

using coro::net::socket_t;
using coro::net::Socket;

static constexpr coro::u8_t EVNO = 0x00;
static constexpr coro::u8_t EVRD = 0x01;
static constexpr coro::u8_t EVWR = 0x02;

class Conn;
using ConnPtr = std::shared_ptr<Conn>;

class Conn final : private coro::UnCopyable {
  Socket socket_;
  coro::net::Status status_{coro::net::Status::INIT_ACK};
  coro::msg::WriteBuf wbuf_;
public:
  Conn(Socket s) noexcept : socket_(s) {
    socket_.set_nonblocking();
  }
  ~Conn() noexcept { close(); }

  inline bool is_valid() const noexcept { return socket_.is_valid(); }
  inline socket_t get_sockfd() const noexcept { return socket_.get(); }
  inline void close() noexcept { socket_.close(); }

  bool do_accept(std::function<void (ConnPtr)>&& fn) {
    if (auto s = socket_.async_accept(); s) {
      if ((*s).is_valid()) {
        auto c = std::make_shared<Conn>(std::move(*s));
        c->on_event_new_connection();
        fn(c);
      }
      return true;
    }
    return false;
  }

  void on_event_new_connection() {
    status_ = coro::net::Status::INIT_ACK;
    wbuf_.clear();
    wbuf_.push_back('*');
  }

  bool on_event_rd_message() {
    if (status_ == coro::net::Status::INIT_ACK || !wbuf_.empty())
      return true;

    char rbuf[1024];
    if (auto n = socket_.async_read(rbuf, sizeof(rbuf)); n) {
      if (auto rlen = *n; rlen > 0) {
        auto [st, wbuf] = coro::msg::handle_message(status_, rbuf, rlen);
        status_ = st;
        if (!wbuf.empty())
          wbuf_.insert(wbuf_.end(), wbuf.begin(), wbuf.end());
      }
      return true;
    }

    close();
    return false;
  }

  bool on_event_wr_message() {
    if (wbuf_.empty())
      return true;

    if (auto n = socket_.async_write(wbuf_.data(), wbuf_.size()); n) {
      if (auto wlen = *n; wlen > 0) {
        if (wlen < wbuf_.size()) {
          wbuf_.erase(wbuf_.begin(), wbuf_.begin() + wlen);
        }
        else {
          wbuf_.clear();
          if (status_ == coro::net::Status::INIT_ACK)
            status_ = coro::net::Status::WAIT_MSG;
        }
      }
      return true;
    }

    close();
    return false;
  }
};

class EventLoop final : private coro::UnCopyable {
  int max_fd_{};
  Socket acceptor_;
  std::list<ConnPtr> conns_;
  fd_set rfds_master_;
  fd_set wfds_master_;

  void add_event(socket_t fd, coro::u8_t ev) {
    if (ev & EVRD)
      FD_SET(fd, &rfds_master_);
    if (ev & EVWR)
      FD_SET(fd, &wfds_master_);

    max_fd_ = max(max_fd_, static_cast<int>(fd));
  }

  void del_event(socket_t fd, coro::u8_t ev) {
    if (ev & EVRD)
      FD_CLR(fd, &rfds_master_);
    if (ev & EVWR)
      FD_CLR(fd, &wfds_master_);
  }
public:
  EventLoop() noexcept {
    FD_ZERO(&rfds_master_);
    FD_ZERO(&wfds_master_);
  }

  ~EventLoop() noexcept {
    conns_.clear();
    acceptor_.close();
  }

  bool create_server(
    coro::strv_t host = "0.0.0.0", coro::u16_t port = 5555, int backlog = 5) {
    if (!acceptor_.start_listen(host, port, backlog))
      return false;

    auto c = std::make_shared<Conn>(acceptor_);
    conns_.push_back(c);
    add_event(acceptor_.get(), EVRD);

    return true;
  }

  bool poll() {
    fd_set rfds = rfds_master_;
    fd_set wfds = wfds_master_;

    int nready = ::select(max_fd_, &rfds, &wfds, nullptr, nullptr);
    if (nready < 0)
      return false;

    for (auto it = conns_.begin(); it != conns_.end();) {
      if (nready <= 0)
        break;

      auto& conn = *it;
      auto sockfd = conn->get_sockfd();
      bool been_sloved{};
      if (conn->is_valid() && FD_ISSET(sockfd, &rfds)) {
        been_sloved = true;
        if (acceptor_ == sockfd) {
          auto r = conn->do_accept([this](ConnPtr c) {
            conns_.push_back(c);
            add_event(c->get_sockfd(), EVRD | EVWR);
          });
          if (!r)
            return false;
        }
        else {
          if (!conn->on_event_rd_message())
            del_event(sockfd, EVRD | EVWR);
        }
      }
      if (conn->is_valid() && FD_ISSET(sockfd, &wfds)) {
        been_sloved = true;
        if (!conn->on_event_wr_message())
          del_event(sockfd, EVRD | EVWR);
      }

      if (been_sloved)
        --nready;

      if (!conn->is_valid())
        conns_.erase(it++);
      else
        ++it;
    }
    return true;
  }

  void run() {
    for (;;) {
      if (!poll())
        break;
    }
  }
};

void launch() {
  coro::net::Initializer<> init;

  EventLoop loop;
  loop.create_server();

  loop.run();
}

}

CORO_EXAMPLE(EchoServerSelect, esel, "an easy echo server with select") {
  echo_server_sel::launch();
}