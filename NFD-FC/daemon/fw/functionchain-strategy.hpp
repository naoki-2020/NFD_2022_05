/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014-2016,  Regents of the University of California,
 *                           Arizona Board of Regents,
 *                           Colorado State University,
 *                           University Pierre & Marie Curie, Sorbonne University,
 *                           Washington University in St. Louis,
 *                           Beijing Institute of Technology,
 *                           The University of Memphis.
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NFD_DAEMON_FW_FUNCTIONCHAIN_STRATEGY_HPP
#define NFD_DAEMON_FW_FUNCTIONCHAIN_STRATEGY_HPP

#include "strategy.hpp"
#include "process-nack-traits.hpp"
#include "retx-suppression-exponential.hpp"


namespace nfd {
namespace fw {

class FunctionChainStrategy : public Strategy
                       , public ProcessNackTraits<FunctionChainStrategy>
{
public:
  explicit
  FunctionChainStrategy(Forwarder& forwarder, const Name& name = getStrategyName());

  static const Name&
  getStrategyName();

  void
  afterReceiveInterest(const FaceEndpoint& ingress, const Interest& interest,
                       const shared_ptr<pit::Entry>& pitEntry) override;

  void
  afterReceiveNack(const FaceEndpoint& ingress, const lp::Nack& nack,
                   const shared_ptr<pit::Entry>& pitEntry) override;

NFD_PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  static const time::milliseconds RETX_SUPPRESSION_INITIAL;
  static const time::milliseconds RETX_SUPPRESSION_MAX;
  RetxSuppressionExponential m_retxSuppression;

  friend ProcessNackTraits<FunctionChainStrategy>;


private:
  const fib::Entry&
  lookupFibwithFunction(const pit::Entry& pitEntry, const Interest& interest);

  Forwarder& m_forwarder;
};

/** \brief Best Route strategy version 1
 *
 *  This strategy forwards a new Interest to the lowest-cost nexthop
 *  that is not same as the downstream, and does not violate scope.
 *  Subsequent similar Interests or consumer retransmissions are suppressed
 *  until after InterestLifetime expiry.
 *
 *  \note This strategy is superceded by Best Route strategy version 2,
 *        which allows consumer retransmissions. This version is kept for
 *        comparison purposes and is not recommended for general usage.
 */


} // namespace fw
} // namespace nfd

#endif // NFD_DAEMON_FW_BEST_ROUTE_STRATEGY_HPP