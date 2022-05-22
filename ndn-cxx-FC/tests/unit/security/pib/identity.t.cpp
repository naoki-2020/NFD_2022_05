/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 */

#include "ndn-cxx/security/pib/identity.hpp"
#include "ndn-cxx/security/pib/impl/identity-impl.hpp"

#include "tests/boost-test.hpp"
#include "tests/unit/security/pib/pib-data-fixture.hpp"

namespace ndn {
namespace security {
namespace pib {
namespace tests {

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(Pib)
BOOST_FIXTURE_TEST_SUITE(TestIdentity, PibDataFixture)

BOOST_AUTO_TEST_CASE(ValidityChecking)
{
  Identity id;
  BOOST_TEST(!id);
  BOOST_TEST(id == Identity());

  auto impl = std::make_shared<detail::IdentityImpl>(id1, makePibWithIdentity(id1));
  id = Identity(impl);
  BOOST_TEST(id);
  BOOST_TEST(id != Identity());

  impl.reset();
  BOOST_TEST(!id);
}

// pib::Identity is a wrapper of pib::detail::IdentityImpl. Since the functionality
// of IdentityImpl is already tested in identity-impl.t.cpp, we only test the shared
// property of pib::Identity in this test case.
BOOST_AUTO_TEST_CASE(SharedImpl)
{
  auto impl = std::make_shared<detail::IdentityImpl>(id1, makePibWithIdentity(id1));
  Identity identity1(impl);
  Identity identity2(impl);

  BOOST_TEST(identity1 == identity2);
  BOOST_TEST(identity1 != Identity());
  BOOST_TEST(Identity() != identity2);
  BOOST_TEST(Identity() == Identity());

  BOOST_CHECK_THROW(identity2.getKey(id1Key1Name), pib::Pib::Error);
  identity1.addKey(id1Key1, id1Key1Name);
  BOOST_TEST(identity2.getKey(id1Key1Name));

  identity2.removeKey(id1Key1Name);
  BOOST_CHECK_THROW(identity1.getKey(id1Key1Name), pib::Pib::Error);

  identity1.setDefaultKey(id1Key1, id1Key1Name);
  BOOST_TEST(identity2.getDefaultKey().getName() == id1Key1Name);
}

BOOST_AUTO_TEST_SUITE_END() // TestIdentity
BOOST_AUTO_TEST_SUITE_END() // Pib
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace pib
} // namespace security
} // namespace ndn
