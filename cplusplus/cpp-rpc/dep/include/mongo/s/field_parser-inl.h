/**
 *    Copyright (C) 2012 10gen Inc.
 *
 *    This program is free software: you can redistribute it and/or  modify
 *    it under the terms of the GNU Affero General Public License, version 3,
 *    as published by the Free Software Foundation.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 *
 *    You should have received a copy of the GNU Affero General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mongo/s/field_parser.h"
#include "mongo/util/mongoutils/str.h"

namespace mongo {

    using mongoutils::str::stream;

    // Extracts an array into a vector
    template<typename T>
    bool FieldParser::extract(BSONObj doc,
                              const BSONField<vector<T> >& field,
                              const vector<T>& def,
                              vector<T>* out,
                              string* errMsg)
    {
        BSONElement elem = doc[field.name()];
        if (elem.eoo()) {
            *out = def;
            return true;
        }

        if (elem.type() == Array) {
            BSONArray arr = BSONArray(elem.embeddedObject());
            string elErrMsg;

            // Append all the new elements to the end of the vector
            size_t initialSize = out->size();
            out->resize(initialSize + arr.nFields());

            int i = 0;
            BSONObjIterator objIt(arr);
            while (objIt.more()) {
                BSONElement next = objIt.next();
                BSONField<T> fieldFor(next.fieldName());

                if (!FieldParser::extract(arr,
                                          fieldFor,
                                          out->at(initialSize + i),
                                          &out->at(initialSize + i),
                                          &elErrMsg))
                {
                    if (errMsg) {
                        *errMsg = stream() << "error parsing element " << i << " of field "
                                           << field() << causedBy(elErrMsg);
                    }
                    return false;
                }
                i++;
            }

            return true;
        }

        if (errMsg) {
            *errMsg = stream() << "wrong type for '" << field() << "' field, expected "
                               << "vector array" << ", found " << doc[field.name()].toString();
        }
        return false;
    }

    // Extracts an object into a map
    template<typename K, typename T>
    bool FieldParser::extract(BSONObj doc,
                              const BSONField<map<K, T> >& field,
                              const map<K, T>& def,
                              map<K, T>* out,
                              string* errMsg)
    {
        BSONElement elem = doc[field.name()];
        if (elem.eoo()) {
            *out = def;
            return true;
        }

        if (elem.type() == Object) {
            BSONObj obj = elem.embeddedObject();
            string elErrMsg;

            BSONObjIterator objIt(obj);
            while (objIt.more()) {
                BSONElement next = objIt.next();
                BSONField<T> fieldFor(next.fieldName());

                T& value = (*out)[next.fieldName()];
                if (!FieldParser::extract(obj, fieldFor, value, &value, &elErrMsg)) {
                    if (errMsg) {
                        *errMsg = stream() << "error parsing map element " << next.fieldName()
                                           << " of field " << field() << causedBy(elErrMsg);
                    }
                    return false;
                }
            }

            return true;
        }

        if (errMsg) {
            *errMsg = stream() << "wrong type for '" << field() << "' field, expected "
                               << "vector array" << ", found " << doc[field.name()].toString();
        }
        return false;
    }

} // namespace mongo
