/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 * Copyright (c) 2016, University of Padova, Dep. of Information Engineering, SIGNET lab
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Nicola Baldo <nbaldo@cttc.es>
 *
 * Modified by: Michele Polese <michele.polese@gmail.com>
 *          Dual Connectivity functionalities
 */

#include "ns3/lte-rlc.h"

#include "ns3/log.h"
#include "ns3/lte-rlc-tag.h"
#include "ns3/simulator.h"
// #include "lte-mac-sap.h"
#include "ns3/lte-rlc-sap.h"

// #include "ff-mac-sched-sap.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("LteRlc");

/// LteRlcSpecificLteMacSapUser class
/*
class LteRlcSpecificLteMacSapUser : public LteMacSapUser
{
public:
*/
/**
 * Constructor
 *
 * \param rlc the RLC
 */
/*
LteRlcSpecificLteMacSapUser (LteRlc* rlc);

// Interface implemented from LteMacSapUser
virtual void NotifyTxOpportunity (LteMacSapUser::TxOpportunityParameters params);
virtual void NotifyHarqDeliveryFailure ();
virtual void NotifyHarqDeliveryFailure (uint8_t harqId);
virtual void ReceivePdu (LteMacSapUser::ReceivePduParameters params);

private:
LteRlcSpecificLteMacSapUser ();
LteRlc* m_rlc; ///< the RLC
};
*/

LteRlcSpecificLteMacSapUser::LteRlcSpecificLteMacSapUser(LteRlc* rlc)
    : m_rlc(rlc)
{
}

LteRlcSpecificLteMacSapUser::LteRlcSpecificLteMacSapUser()
{
}

void
LteRlcSpecificLteMacSapUser::NotifyTxOpportunity(TxOpportunityParameters params)
{
    m_rlc->DoNotifyTxOpportunity(params);
}

void
LteRlcSpecificLteMacSapUser::NotifyHarqDeliveryFailure()
{
    m_rlc->DoNotifyHarqDeliveryFailure();
}

void
LteRlcSpecificLteMacSapUser::NotifyHarqDeliveryFailure(uint8_t harqId)
{
    m_rlc->DoNotifyHarqDeliveryFailure(harqId);
}

void
LteRlcSpecificLteMacSapUser::ReceivePdu(LteMacSapUser::ReceivePduParameters params)
{
    m_rlc->DoReceivePdu(params);
}

///////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED(LteRlc);

LteRlc::LteRlc()
    : m_rlcSapUser(0),
      m_macSapProvider(0),
      m_rnti(0),
      m_lcid(0),
      m_imsi(0),
      isMc(false), // TODO refactor this!!
      m_txPacketsInReportingPeriod(0),
      m_txBytesInReportingPeriod(0)
{
    NS_LOG_FUNCTION(this);
    m_rlcSapProvider = new LteRlcSpecificLteRlcSapProvider<LteRlc>(this);
    m_epcX2RlcUser = new EpcX2RlcSpecificUser<LteRlc>(this);
    m_macSapUser = new LteRlcSpecificLteMacSapUser(this);
}

LteRlc::~LteRlc()
{
    NS_LOG_FUNCTION(this);
}

TypeId
LteRlc::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::LteRlc")
                            .SetParent<Object>()
                            .SetGroupName("Lte")
                            .AddTraceSource("TxPDU",
                                            "PDU transmission notified to the MAC.",
                                            MakeTraceSourceAccessor(&LteRlc::m_txPdu),
                                            "ns3::LteRlc::NotifyTxTracedCallback")
                            .AddTraceSource("RxPDU",
                                            "PDU received.",
                                            MakeTraceSourceAccessor(&LteRlc::m_rxPdu),
                                            "ns3::LteRlc::ReceiveTracedCallback")
                            .AddTraceSource("TxCompletedCallback",
                                            "PDU acked.",
                                            MakeTraceSourceAccessor(&LteRlc::m_txCompletedCallback),
                                            "ns3::LteRlc::RetransmissionCountCallback");
    return tid;
}

void
LteRlc::DoDispose()
{
    NS_LOG_FUNCTION(this);
    delete m_rlcSapProvider;
    delete m_epcX2RlcUser;
    delete m_macSapUser;
}

void
LteRlc::SetRnti(uint16_t rnti)
{
    NS_LOG_FUNCTION(this << (uint32_t)rnti);
    m_rnti = rnti;
}

void
LteRlc::SetLcId(uint8_t lcId)
{
    NS_LOG_FUNCTION(this << (uint32_t)lcId);
    m_lcid = lcId;
}

void
LteRlc::SetImsi(uint64_t imsi)
{
    NS_LOG_FUNCTION(this << imsi);
    m_imsi = imsi;
}

void
LteRlc::SetLteRlcSapUser(LteRlcSapUser* s)
{
    NS_LOG_FUNCTION(this << s);
    m_rlcSapUser = s;
}

LteRlcSapProvider*
LteRlc::GetLteRlcSapProvider()
{
    NS_LOG_FUNCTION(this);
    return m_rlcSapProvider;
}

void
LteRlc::SetLteMacSapProvider(LteMacSapProvider* s)
{
    NS_LOG_FUNCTION(this << s);
    m_macSapProvider = s;
}

LteMacSapUser*
LteRlc::GetLteMacSapUser()
{
    NS_LOG_FUNCTION(this);
    return m_macSapUser;
}

void
LteRlc::DoNotifyHarqDeliveryFailure(uint8_t harqId)
{
    NS_LOG_FUNCTION(this);
}

void
LteRlc::SetUeDataParams(EpcX2Sap::UeDataParams params)
{
    isMc = true;
    m_ueDataParams = params;
}

void
LteRlc::SetEpcX2RlcProvider(EpcX2RlcProvider* s)
{
    m_epcX2RlcProvider = s;
}

EpcX2RlcUser*
LteRlc::GetEpcX2RlcUser()
{
    return m_epcX2RlcUser;
}

////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED(LteRlcSm);

LteRlcSm::LteRlcSm()
{
    NS_LOG_FUNCTION(this);
}

LteRlcSm::~LteRlcSm()
{
    NS_LOG_FUNCTION(this);
}

TypeId
LteRlcSm::GetTypeId(void)
{
    static TypeId tid =
        TypeId("ns3::LteRlcSm").SetParent<LteRlc>().SetGroupName("Lte").AddConstructor<LteRlcSm>();
    return tid;
}

void
LteRlcSm::DoInitialize()
{
    NS_LOG_FUNCTION(this);
    ReportBufferStatus();
}

void
LteRlcSm::DoDispose()
{
    NS_LOG_FUNCTION(this);
    LteRlc::DoDispose();
}

void
LteRlcSm::DoTransmitPdcpPdu(Ptr<Packet> p)
{
    NS_LOG_FUNCTION(this << p);
}

void
LteRlcSm::DoReceivePdu(LteMacSapUser::ReceivePduParameters rxPduParams)
{
    NS_LOG_FUNCTION(this << rxPduParams.p);
    // RLC Performance evaluation
    RlcTag rlcTag;
    Time delay;
    if (rxPduParams.p->FindFirstMatchingByteTag(rlcTag))
    {
        delay = Simulator::Now() - rlcTag.GetSenderTimestamp();
    }
    NS_LOG_LOGIC(" RNTI=" << m_rnti << " LCID=" << (uint32_t)m_lcid << " size="
                          << rxPduParams.p->GetSize() << " delay=" << delay.GetNanoSeconds());
    m_rxPdu(m_rnti, m_lcid, rxPduParams.p->GetSize(), delay.GetNanoSeconds());
}

void
LteRlcSm::DoNotifyTxOpportunity(LteMacSapUser::TxOpportunityParameters txOpParams)
{
    NS_LOG_FUNCTION(this << txOpParams.bytes);
    LteMacSapProvider::TransmitPduParameters params;
    params.pdu = Create<Packet>(txOpParams.bytes);
    params.rnti = m_rnti;
    params.lcid = m_lcid;
    params.layer = txOpParams.layer;
    params.harqProcessId = txOpParams.harqId;
    params.componentCarrierId = txOpParams.componentCarrierId;

    // RLC Performance evaluation
    RlcTag tag(Simulator::Now());
    params.pdu->AddByteTag(tag);
    NS_LOG_LOGIC(" RNTI=" << m_rnti << " LCID=" << (uint32_t)m_lcid
                          << " size=" << txOpParams.bytes);
    m_txPdu(m_rnti, m_lcid, txOpParams.bytes);

    m_macSapProvider->TransmitPdu(params);
    ReportBufferStatus();
}

void
LteRlcSm::DoNotifyHarqDeliveryFailure()
{
    NS_LOG_FUNCTION(this);
}

void
LteRlcSm::ReportBufferStatus()
{
    NS_LOG_FUNCTION(this);
    LteMacSapProvider::ReportBufferStatusParameters p;
    p.rnti = m_rnti;
    p.lcid = m_lcid;
    p.txQueueSize = 1000000; // mmWave module: Arbitrarily changed full-buffer BSR to report 1MB
                             // available each subframe
    p.txQueueHolDelay = 10;
    p.retxQueueSize = 0;
    p.retxQueueHolDelay = 0;
    p.statusPduSize = 0;
    p.arrivalRate = 0;
    m_macSapProvider->ReportBufferStatus(p);
}

void
LteRlcSm::DoSendMcPdcpSdu(EpcX2Sap::UeDataParams params)
{
    NS_LOG_FUNCTION(this);
    NS_FATAL_ERROR("Not supported");
}

//////////////////////////////////////////

// LteRlcTm::~LteRlcTm ()
// {
// }

//////////////////////////////////////////

// LteRlcUm::~LteRlcUm ()
// {
// }

//////////////////////////////////////////

// LteRlcAm::~LteRlcAm ()
// {
// }

} // namespace ns3
