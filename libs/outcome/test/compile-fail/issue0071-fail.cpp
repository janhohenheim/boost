/* clang-format off
(error: no matching function for call to .+::basic_result|error: no matching constructor for initialization of 'result<udt>'|cannot convert argument 1 from 'int'|no overloaded function could convert all the argument types)
clang-format on


Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License in the accompanying file
Licence.txt or at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.


Distributed under the Boost Software License, Version 1.0.
(See accompanying file Licence.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#include "../../include/boost/outcome/result.hpp"

int main()
{
  using namespace BOOST_OUTCOME_V2_NAMESPACE;
  struct udt
  {
    explicit udt(int /*unused*/) {}
  };
  // Must not be possible to implicitly initialise a result<udt>
  result<udt> m(5);
  (void) m;
  return 0;
}
