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

#include "ndn-cxx/security/pib/impl/key-impl.hpp"
#include "ndn-cxx/security/pib/pib-impl.hpp"
#include "ndn-cxx/security/transform/public-key.hpp"
#include "ndn-cxx/util/logger.hpp"

namespace ndn {
namespace security {
namespace pib {
namespace detail {

NDN_LOG_INIT(ndn.security.Key);

KeyImpl::KeyImpl(const Name& keyName, Buffer key, shared_ptr<PibImpl> pibImpl)
  : m_identity(extractIdentityFromKeyName(keyName))
  , m_keyName(keyName)
  , m_key(std::move(key))
  , m_pib(std::move(pibImpl))
  , m_certificates(m_keyName, m_pib)
{
  BOOST_ASSERT(m_pib != nullptr);
  BOOST_ASSERT(m_pib->hasKey(m_keyName));

  transform::PublicKey publicKey;
  try {
    publicKey.loadPkcs8(m_key);
  }
  catch (const transform::PublicKey::Error&) {
    NDN_THROW_NESTED(std::invalid_argument("Invalid key bits"));
  }
  m_keyType = publicKey.getKeyType();
}

void
KeyImpl::addCertificate(const Certificate& cert)
{
  BOOST_ASSERT(m_certificates.isConsistent());
  m_certificates.add(cert);
}

void
KeyImpl::removeCertificate(const Name& certName)
{
  BOOST_ASSERT(m_certificates.isConsistent());

  if (m_defaultCert && m_defaultCert->getName() == certName) {
    NDN_LOG_DEBUG("Removing default certificate " << certName);
    m_defaultCert = nullopt;
  }
  m_certificates.remove(certName);
}

const Certificate&
KeyImpl::setDefaultCert(Certificate cert)
{
  BOOST_ASSERT(m_certificates.isConsistent());

  m_defaultCert = std::move(cert);
  m_pib->setDefaultCertificateOfKey(m_keyName, m_defaultCert->getName());
  NDN_LOG_DEBUG("Default certificate set to " << m_defaultCert->getName());

  return *m_defaultCert;
}

const Certificate&
KeyImpl::getDefaultCertificate() const
{
  BOOST_ASSERT(m_certificates.isConsistent());

  if (!m_defaultCert) {
    m_defaultCert = m_pib->getDefaultCertificateOfKey(m_keyName);
    NDN_LOG_DEBUG("Caching default certificate " << m_defaultCert->getName());
  }

  BOOST_ASSERT(m_defaultCert);
  BOOST_ASSERT(m_defaultCert->getName() == m_pib->getDefaultCertificateOfKey(m_keyName).getName());
  return *m_defaultCert;
}

} // namespace detail
} // namespace pib
} // namespace security
} // namespace ndn
