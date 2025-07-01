/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *   Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *   Copyright (c) 2015, NYU WIRELESS, Tandon School of Engineering, New York University
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *   Author: Marco Miozzo <marco.miozzo@cttc.es>
 *           Nicola Baldo  <nbaldo@cttc.es>
 *
 *   Modified by: Marco Mezzavilla <mezzavilla@nyu.edu>
 *                Sourjya Dutta <sdutta@nyu.edu>
 *                Russell Ford <russell.ford@nyu.edu>
 *                Menglei Zhang <menglei@nyu.edu>
 */

#include "mmwave-flex-tti-mac-scheduler.h"
#include "mmwave-mac-pdu-header.h"
#include "mmwave-mac-pdu-tag.h"
#include "mmwave-spectrum-value-helper.h"
#include <ns3/abort.h>
#include <ns3/boolean.h>
#include <ns3/log.h>
#include <ns3/lte-common.h>
#include <cmath>
#include <iostream>
#include <vector>

namespace ns3 {

namespace mmwave {

NS_LOG_COMPONENT_DEFINE ("MmWaveFlexTtiMacScheduler");

NS_OBJECT_ENSURE_REGISTERED (MmWaveFlexTtiMacScheduler);

class MmWaveFlexTtiMacCschedSapProvider : public MmWaveMacCschedSapProvider
{
public:
  MmWaveFlexTtiMacCschedSapProvider (MmWaveFlexTtiMacScheduler* scheduler);
  virtual void CschedCellConfigReq (const MmWaveMacCschedSapProvider::CschedCellConfigReqParameters& params);
  virtual void CschedUeConfigReq (const MmWaveMacCschedSapProvider::CschedUeConfigReqParameters& params);
  virtual void CschedLcConfigReq (const MmWaveMacCschedSapProvider::CschedLcConfigReqParameters& params);
  virtual void CschedLcReleaseReq (const MmWaveMacCschedSapProvider::CschedLcReleaseReqParameters& params);
  virtual void CschedUeReleaseReq (const MmWaveMacCschedSapProvider::CschedUeReleaseReqParameters& params);

private:
  MmWaveFlexTtiMacCschedSapProvider ();
  MmWaveFlexTtiMacScheduler* m_scheduler;
};

MmWaveFlexTtiMacCschedSapProvider::MmWaveFlexTtiMacCschedSapProvider ()
{
}

MmWaveFlexTtiMacCschedSapProvider::MmWaveFlexTtiMacCschedSapProvider (MmWaveFlexTtiMacScheduler* scheduler)
  : m_scheduler (scheduler)
{
}

void
MmWaveFlexTtiMacCschedSapProvider::CschedCellConfigReq (const MmWaveMacCschedSapProvider::CschedCellConfigReqParameters& params)
{
  m_scheduler->DoCschedCellConfigReq (params);
}

void
MmWaveFlexTtiMacCschedSapProvider::CschedUeConfigReq (const MmWaveMacCschedSapProvider::CschedUeConfigReqParameters& params)
{
  m_scheduler->DoCschedUeConfigReq (params);
}

void
MmWaveFlexTtiMacCschedSapProvider::CschedLcConfigReq (const MmWaveMacCschedSapProvider::CschedLcConfigReqParameters& params)
{
  m_scheduler->DoCschedLcConfigReq (params);
}

void
MmWaveFlexTtiMacCschedSapProvider::CschedLcReleaseReq (const MmWaveMacCschedSapProvider::CschedLcReleaseReqParameters& params)
{
  m_scheduler->DoCschedLcReleaseReq (params);
}

void
MmWaveFlexTtiMacCschedSapProvider::CschedUeReleaseReq (const MmWaveMacCschedSapProvider::CschedUeReleaseReqParameters& params)
{
  m_scheduler->DoCschedUeReleaseReq (params);
}

class MmWaveFlexTtiMacSchedSapProvider : public MmWaveMacSchedSapProvider
{
public:
  MmWaveFlexTtiMacSchedSapProvider (MmWaveFlexTtiMacScheduler* sched);
  virtual void SchedDlRlcBufferReq (const MmWaveMacSchedSapProvider::SchedDlRlcBufferReqParameters& params);
  virtual void SchedTriggerReq (const MmWaveMacSchedSapProvider::SchedTriggerReqParameters& params);
  virtual void SchedDlCqiInfoReq (const MmWaveMacSchedSapProvider::SchedDlCqiInfoReqParameters& params);
  virtual void SchedUlCqiInfoReq (const MmWaveMacSchedSapProvider::SchedUlCqiInfoReqParameters& params);
  virtual void SchedUlMacCtrlInfoReq (const MmWaveMacSchedSapProvider::SchedUlMacCtrlInfoReqParameters& params);
  virtual void SchedSetMcs (int mcs);

private:
  MmWaveFlexTtiMacSchedSapProvider ();
  MmWaveFlexTtiMacScheduler* m_scheduler;
};

MmWaveFlexTtiMacSchedSapProvider::MmWaveFlexTtiMacSchedSapProvider ()
{
}

MmWaveFlexTtiMacSchedSapProvider::MmWaveFlexTtiMacSchedSapProvider (MmWaveFlexTtiMacScheduler* sched)
  : m_scheduler (sched)
{
}

void
MmWaveFlexTtiMacSchedSapProvider::SchedDlRlcBufferReq (const MmWaveMacSchedSapProvider::SchedDlRlcBufferReqParameters& params)
{
  m_scheduler->DoSchedDlRlcBufferReq (params);
}

void
MmWaveFlexTtiMacSchedSapProvider::SchedTriggerReq (const MmWaveMacSchedSapProvider::SchedTriggerReqParameters& params)
{
  m_scheduler->DoSchedTriggerReq (params);
}

void
MmWaveFlexTtiMacSchedSapProvider::SchedDlCqiInfoReq (const MmWaveMacSchedSapProvider::SchedDlCqiInfoReqParameters& params)
{
  m_scheduler->DoSchedDlCqiInfoReq (params);
}

void
MmWaveFlexTtiMacSchedSapProvider::SchedUlCqiInfoReq (const MmWaveMacSchedSapProvider::SchedUlCqiInfoReqParameters& params)
{
  m_scheduler->DoSchedUlCqiInfoReq (params);
}

void
MmWaveFlexTtiMacSchedSapProvider::SchedUlMacCtrlInfoReq (const MmWaveMacSchedSapProvider::SchedUlMacCtrlInfoReqParameters& params)
{
  m_scheduler->DoSchedUlMacCtrlInfoReq (params);
}

void
MmWaveFlexTtiMacSchedSapProvider::SchedSetMcs (int mcs)
{
  m_scheduler->DoSchedSetMcs (mcs);
}

MmWaveFlexTtiMacScheduler::MmWaveFlexTtiMacScheduler ()
  : m_nextRnti (0),
    m_nextRntiDl (0),
    m_nextRntiUl (0),
    m_tbUid (0),
    m_macSchedSapUser (nullptr),
    m_macCschedSapUser (nullptr)
{
  NS_LOG_FUNCTION (this);
  m_macSchedSapProvider = new MmWaveFlexTtiMacSchedSapProvider (this);
  m_macCschedSapProvider = new MmWaveFlexTtiMacCschedSapProvider (this);
}

MmWaveFlexTtiMacScheduler::~MmWaveFlexTtiMacScheduler ()
{
  NS_LOG_FUNCTION (this);
}

void
MmWaveFlexTtiMacScheduler::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_wbCqiRxed.clear ();
  m_dlHarqProcessesDciInfoMap.clear ();
  m_dlHarqProcessesTimer.clear ();
  m_dlHarqProcessesRlcPduMap.clear ();
  m_dlHarqInfoList.clear ();
  m_ulHarqCurrentProcessId.clear ();
  m_ulHarqProcessesStatus.clear ();
  m_ulHarqProcessesTimer.clear ();
  m_ulHarqProcessesDciInfoMap.clear ();
  delete m_macCschedSapProvider;
  delete m_macSchedSapProvider;
}

TypeId
MmWaveFlexTtiMacScheduler::GetTypeId (void)
{
  static TypeId tid =
    TypeId ("ns3::MmWaveFlexTtiMacScheduler")
    .SetParent<MmWaveMacScheduler> ()
    .AddConstructor<MmWaveFlexTtiMacScheduler> ()
    .AddAttribute ("CqiTimerThreshold",
                   "The number of TTIs a CQI is valid (default 1000 - 1 sec.)",
                   UintegerValue (100),
                   MakeUintegerAccessor (&MmWaveFlexTtiMacScheduler::m_cqiTimersThreshold),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("HarqEnabled",
                   "Activate/Deactivate the HARQ [by default is active].",
                   BooleanValue (true),
                   MakeBooleanAccessor (&MmWaveFlexTtiMacScheduler::m_harqOn),
                   MakeBooleanChecker ())
    .AddAttribute ("FixedMcsDl",
                   "Fix MCS to value set in McsDlDefault (for testing)",
                   BooleanValue (false),
                   MakeBooleanAccessor (&MmWaveFlexTtiMacScheduler::m_fixedMcsDl),
                   MakeBooleanChecker ())
    .AddAttribute ("McsDefaultDl",
                   "Fixed DL MCS (for testing)",
                   UintegerValue (1),
                   MakeUintegerAccessor (&MmWaveFlexTtiMacScheduler::m_mcsDefaultDl),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("FixedMcsUl",
                   "Fix MCS to value set in McsUlDefault (for testing)",
                   BooleanValue (false),
                   MakeBooleanAccessor (&MmWaveFlexTtiMacScheduler::m_fixedMcsUl),
                   MakeBooleanChecker ())
    .AddAttribute ("McsDefaultUl",
                   "Fixed UL MCS (for testing)",
                   UintegerValue (1),
                   MakeUintegerAccessor (&MmWaveFlexTtiMacScheduler::m_mcsDefaultUl),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("DlSchedOnly",
                   "Only schedule downlink traffic (for testing)",
                   BooleanValue (false),
                   MakeBooleanAccessor (&MmWaveFlexTtiMacScheduler::m_dlOnly),
                   MakeBooleanChecker ())
    .AddAttribute ("UlSchedOnly",
                   "Only schedule uplink traffic (for testing)",
                   BooleanValue (false),
                   MakeBooleanAccessor (&MmWaveFlexTtiMacScheduler::m_ulOnly),
                   MakeBooleanChecker ())
    .AddAttribute ("FixedTti",
                   "Fix slot size",
                   BooleanValue (false),
                   MakeBooleanAccessor (&MmWaveFlexTtiMacScheduler::m_fixedTti),
                   MakeBooleanChecker ())
    .AddAttribute ("SymPerSlot",
                   "Number of symbols per slot in Fixed TTI mode",
                   UintegerValue (6),
                   MakeUintegerAccessor (&MmWaveFlexTtiMacScheduler::m_symPerSlot),
                   MakeUintegerChecker<uint8_t> ());

  return tid;
}

void
MmWaveFlexTtiMacScheduler::SetMacSchedSapUser (MmWaveMacSchedSapUser* sap)
{
  m_macSchedSapUser = sap;
}

void
MmWaveFlexTtiMacScheduler::SetMacCschedSapUser (MmWaveMacCschedSapUser* sap)
{
  m_macCschedSapUser = sap;
}

MmWaveMacSchedSapProvider*
MmWaveFlexTtiMacScheduler::GetMacSchedSapProvider ()
{
  return m_macSchedSapProvider;
}

MmWaveMacCschedSapProvider*
MmWaveFlexTtiMacScheduler::GetMacCschedSapProvider ()
{
  return m_macCschedSapProvider;
}

void
MmWaveFlexTtiMacScheduler::ConfigureCommonParameters (Ptr<MmWavePhyMacCommon> config)
{
  m_phyMacConfig = config;
  m_amc = CreateObject<MmWaveAmc> (m_phyMacConfig);
  m_numHarqProcess = m_phyMacConfig->GetNumHarqProcess ();
  m_harqTimeout = m_phyMacConfig->GetHarqTimeout ();
  m_numDataSymbols = m_phyMacConfig->GetSymbPerSlot () - m_phyMacConfig->GetDlCtrlSymbols () -
                     m_phyMacConfig->GetUlCtrlSymbols ();
}

void
MmWaveFlexTtiMacScheduler::DoCschedCellConfigReq (const MmWaveMacCschedSapProvider::CschedCellConfigReqParameters& params)
{
  NS_LOG_FUNCTION (this);
}

void
MmWaveFlexTtiMacScheduler::DoCschedUeConfigReq (const MmWaveMacCschedSapProvider::CschedUeConfigReqParameters& params)
{
  NS_LOG_FUNCTION (this << " RNTI " << params.m_rnti);

  if (m_dlHarqProcessesStatus.find (params.m_rnti) == m_dlHarqProcessesStatus.end ())
    {
      m_dlHarqCurrentProcessId.insert (std::pair<uint16_t, uint8_t> (params.m_rnti, 0));
      DlHarqProcessesStatus_t dlHarqPrcStatus;
      dlHarqPrcStatus.resize (m_phyMacConfig->GetNumHarqProcess (), 0);
      m_dlHarqProcessesStatus.insert (std::pair<uint16_t, DlHarqProcessesStatus_t> (params.m_rnti, dlHarqPrcStatus));
      DlHarqProcessesTimer_t dlHarqProcessesTimer;
      dlHarqProcessesTimer.resize (m_phyMacConfig->GetNumHarqProcess (), 0);
      m_dlHarqProcessesTimer.insert (std::pair<uint16_t, DlHarqProcessesTimer_t> (params.m_rnti, dlHarqProcessesTimer));
      DlHarqProcessesDciInfoList_t dlHarqTbInfoList;
      dlHarqTbInfoList.resize (m_phyMacConfig->GetNumHarqProcess ());
      m_dlHarqProcessesDciInfoMap.insert (std::pair<uint16_t, DlHarqProcessesDciInfoList_t> (params.m_rnti, dlHarqTbInfoList));
      DlHarqRlcPduList_t dlHarqRlcPduList;
      dlHarqRlcPduList.resize (m_phyMacConfig->GetNumHarqProcess ());
      m_dlHarqProcessesRlcPduMap.push_back (std::make_pair (params.m_rnti, dlHarqRlcPduList));
    }

  if (m_ulHarqProcessesStatus.find (params.m_rnti) == m_ulHarqProcessesStatus.end ())
    {
      m_ulHarqCurrentProcessId.insert (std::pair<uint16_t, uint8_t> (params.m_rnti, 0));
      UlHarqProcessesStatus_t ulHarqPrcStatus;
      ulHarqPrcStatus.resize (m_phyMacConfig->GetNumHarqProcess (), 0);
      m_ulHarqProcessesStatus.insert (std::pair<uint16_t, UlHarqProcessesStatus_t> (params.m_rnti, ulHarqPrcStatus));
      UlHarqProcessesTimer_t ulHarqProcessesTimer;
      ulHarqProcessesTimer.resize (m_phyMacConfig->GetNumHarqProcess (), 0);
      m_ulHarqProcessesTimer.insert (std::pair<uint16_t, UlHarqProcessesTimer_t> (params.m_rnti, ulHarqProcessesTimer));
      UlHarqProcessesDciInfoList_t ulHarqTbInfoList;
      ulHarqTbInfoList.resize (m_phyMacConfig->GetNumHarqProcess ());
      m_ulHarqProcessesDciInfoMap.insert (std::pair<uint16_t, UlHarqProcessesDciInfoList_t> (params.m_rnti, ulHarqTbInfoList));
    }
}

void
MmWaveFlexTtiMacScheduler::DoCschedLcConfigReq (const MmWaveMacCschedSapProvider::CschedLcConfigReqParameters& params)
{
  NS_LOG_FUNCTION (this);
}

void
MmWaveFlexTtiMacScheduler::DoCschedLcReleaseReq (const MmWaveMacCschedSapProvider::CschedLcReleaseReqParameters& params)
{
  NS_LOG_FUNCTION (this);
}

void
MmWaveFlexTtiMacScheduler::DoCschedUeReleaseReq (const MmWaveMacCschedSapProvider::CschedUeReleaseReqParameters& params)
{
  NS_LOG_FUNCTION (this);
}

void
MmWaveFlexTtiMacScheduler::DoSchedDlRlcBufferReq (const MmWaveMacSchedSapProvider::SchedDlRlcBufferReqParameters& params)
{
  NS_LOG_FUNCTION (this << params.m_rnti << (uint32_t) params.m_logicalChannelIdentity);
  std::list<MmWaveMacSchedSapProvider::SchedDlRlcBufferReqParameters>::iterator it = m_rlcBufferReq.begin ();
  bool newLc = true;
  while (it != m_rlcBufferReq.end ())
    {
      if (it->m_rnti == params.m_rnti && it->m_logicalChannelIdentity == params.m_logicalChannelIdentity)
        {
          it = m_rlcBufferReq.erase (it);
          newLc = false;
        }
      else
        {
          ++it;
        }
    }
  m_rlcBufferReq.insert (it, params);
  NS_LOG_INFO ("BSR for RNTI " << params.m_rnti << " LC " << (uint16_t) params.m_logicalChannelIdentity
                               << " RLC tx size " << params.m_rlcTransmissionQueueSize
                               << " RLC retx size " << params.m_rlcRetransmissionQueueSize
                               << " RLC stat size " << params.m_rlcStatusPduSize);
  if (newLc)
    {
      m_wbCqiRxed.insert (std::pair<uint16_t, uint8_t> (params.m_rnti, 1));
      m_wbCqiTimers.insert (std::pair<uint16_t, uint32_t> (params.m_rnti, m_cqiTimersThreshold));
    }
}

void
MmWaveFlexTtiMacScheduler::DoSchedDlCqiInfoReq (const MmWaveMacSchedSapProvider::SchedDlCqiInfoReqParameters& params)
{
  NS_LOG_FUNCTION (this);

  for (unsigned int i = 0; i < params.m_cqiList.size (); i++)
    {
      if (params.m_cqiList.at (i).m_cqiType == DlCqiInfo::WB)
        {
          uint16_t rnti = params.m_cqiList.at (i).m_rnti;
          auto it = m_wbCqiRxed.find (rnti);
          if (it == m_wbCqiRxed.end ())
            {
              m_wbCqiRxed.insert (std::pair<uint16_t, uint8_t> (rnti, params.m_cqiList.at (i).m_wbCqi));
              m_wbCqiTimers.insert (std::pair<uint16_t, uint32_t> (rnti, m_cqiTimersThreshold));
            }
          else
            {
              it->second = params.m_cqiList.at (i).m_wbCqi;
              auto itTimers = m_wbCqiTimers.find (rnti);
              itTimers->second = m_cqiTimersThreshold;
            }
        }
      else if (params.m_cqiList.at (i).m_cqiType == DlCqiInfo::SB)
        {
        }
      else
        {
          NS_LOG_ERROR (this << " CQI type unknown");
        }
    }
}

void
MmWaveFlexTtiMacScheduler::DoSchedUlCqiInfoReq (const MmWaveMacSchedSapProvider::SchedUlCqiInfoReqParameters& params)
{
  NS_LOG_FUNCTION (this);

  uint32_t frameNum = params.m_sfnSf.m_frameNum;
  uint8_t subframeNum = params.m_sfnSf.m_sfNum;
  uint8_t slotNum = params.m_sfnSf.m_slotNum;
  uint8_t symNum = params.m_sfnSf.m_symStart;

  switch (params.m_ulCqi.m_type)
    {
    case UlCqiInfo::PUSCH:
      {
        auto itMap = m_ulAllocationMap.find (params.m_sfnSf.Encode ());
        if (itMap == m_ulAllocationMap.end ())
          {
            NS_LOG_INFO (this << " Does not find info on allocation, size: " << m_ulAllocationMap.size ());
            return;
          }
        NS_ASSERT_MSG (itMap->second.m_rntiPerChunk.size () == m_phyMacConfig->GetNumRb (),
                       "SINR chunk map must cover full BW in TDMA mode");
        for (unsigned i = 0; i < itMap->second.m_rntiPerChunk.size (); i++)
          {
            auto itCqi = m_ueUlCqi.find (itMap->second.m_rntiPerChunk.at (i));
            if (itCqi == m_ueUlCqi.end ())
              {
                std::vector<double> newCqi (m_phyMacConfig->GetNumRb (), 30.0);
                newCqi[i] = params.m_ulCqi.m_sinr.at (i);
                m_ueUlCqi.insert (std::pair<uint16_t, UlCqiMapElem> (
                    itMap->second.m_rntiPerChunk.at (i),
                    UlCqiMapElem (newCqi, itMap->second.m_numSym, itMap->second.m_tbSize)));
                m_ueCqiTimers.insert (std::pair<uint16_t, uint32_t> (itMap->second.m_rntiPerChunk.at (i), m_cqiTimersThreshold));
                NS_LOG_INFO ("UL CQI report for RNTI " << itMap->second.m_rntiPerChunk.at (i) << " chunk " << i
                                                       << " SINR " << params.m_ulCqi.m_sinr.at (i) << " frame " << frameNum
                                                       << " subframe " << +subframeNum << " slot " << +slotNum << " startSym " << +symNum);
              }
            else
              {
                itCqi->second.m_ueUlCqi.at (i) = params.m_ulCqi.m_sinr.at (i);
                itCqi->second.m_numSym = itMap->second.m_numSym;
                itCqi->second.m_tbSize = itMap->second.m_tbSize;
                auto itTimers = m_ueCqiTimers.find (itMap->second.m_rntiPerChunk.at (i));
                itTimers->second = m_cqiTimersThreshold;
                NS_LOG_INFO ("UL CQI report for RNTI " << itMap->second.m_rntiPerChunk.at (i) << " chunk " << i
                                                       << " SINR " << params.m_ulCqi.m_sinr.at (i) << " frame " << frameNum
                                                       << " subframe " << +subframeNum << " slot " << +slotNum << " startSym " << +symNum);
              }
          }
        m_ulAllocationMap.erase (itMap);
      }
      break;
    default:
      NS_FATAL_ERROR ("Unknown type of UL-CQI");
    }
}

void
MmWaveFlexTtiMacScheduler::DoSchedUlMacCtrlInfoReq (const MmWaveMacSchedSapProvider::SchedUlMacCtrlInfoReqParameters& params)
{
  NS_LOG_FUNCTION (this);

  for (unsigned int i = 0; i < params.m_macCeList.size (); i++)
    {
      if (params.m_macCeList.at (i).m_macCeType == MacCeElement::BSR)
        {
          uint32_t buffer = 0;
          for (uint8_t lcg = 0; lcg < 4; ++lcg)
            {
              uint8_t bsrId = params.m_macCeList.at (i).m_macCeValue.m_bufferStatus.at (lcg);
              buffer += BsrId2BufferSize (bsrId);
            }

          uint16_t rnti = params.m_macCeList.at (i).m_rnti;
          auto it = m_ceBsrRxed.find (rnti);
          if (it == m_ceBsrRxed.end ())
            {
              m_ceBsrRxed.insert (std::pair<uint16_t, uint32_t> (rnti, buffer));
              NS_LOG_INFO (this << " Insert RNTI " << rnti << " queue " << buffer);
            }
          else
            {
              it->second = buffer;
              NS_LOG_INFO (this << " Update RNTI " << rnti << " queue " << buffer);
            }
        }
    }
}

void
MmWaveFlexTtiMacScheduler::DoSchedSetMcs (int mcs)
{
  if (mcs >= 0 && mcs <= 28)
    {
      m_mcsDefaultDl = mcs;
      m_mcsDefaultUl = mcs;
    }
}

bool
MmWaveFlexTtiMacScheduler::SortRlcBufferReq (MmWaveMacSchedSapProvider::SchedDlRlcBufferReqParameters i,
                                             MmWaveMacSchedSapProvider::SchedDlRlcBufferReqParameters j)
{
  return i.m_rnti < j.m_rnti;
}

void
MmWaveFlexTtiMacScheduler::RefreshDlCqiMaps ()
{
  NS_LOG_FUNCTION (this << m_wbCqiTimers.size ());
  auto itP10 = m_wbCqiTimers.begin ();
  while (itP10 != m_wbCqiTimers.end ())
    {
      if (itP10->second == 0)
        {
          auto itMap = m_wbCqiRxed.find (itP10->first);
          NS_ASSERT_MSG (itMap != m_wbCqiRxed.end (), "Does not find CQI report for user " << itP10->first);
          NS_LOG_INFO (this << " P10-CQI expired for user " << itP10->first);
          m_wbCqiRxed.erase (itMap);
          auto temp = itP10;
          ++itP10;
          m_wbCqiTimers.erase (temp);
        }
      else
        {
          itP10->second--;
          ++itP10;
        }
    }
}

void
MmWaveFlexTtiMacScheduler::RefreshUlCqiMaps ()
{
  auto itUl = m_ueCqiTimers.begin ();
  while (itUl != m_ueCqiTimers.end ())
    {
      if (itUl->second == 0)
        {
          auto itMap = m_ueUlCqi.find (itUl->first);
          if (itMap != m_ueUlCqi.end ())
            {
              NS_LOG_INFO (this << " UL-CQI expired for user " << itUl->first);
              itMap->second.m_ueUlCqi.clear ();
              m_ueUlCqi.erase (itMap);
            }
          auto temp = itUl;
          ++itUl;
          m_ueCqiTimers.erase (temp);
        }
      else
        {
          itUl->second--;
          ++itUl;
        }
    }
}

void
MmWaveFlexTtiMacScheduler::RefreshHarqProcesses ()
{
  NS_LOG_FUNCTION (this);

  for (auto itTimers = m_dlHarqProcessesTimer.begin (); itTimers != m_dlHarqProcessesTimer.end (); ++itTimers)
    {
      for (uint16_t i = 0; i < m_phyMacConfig->GetNumHarqProcess (); i++)
        {
          if (itTimers->second.at (i) == m_phyMacConfig->GetHarqTimeout ())
            {
              auto itStat = m_dlHarqProcessesStatus.find (itTimers->first);
              if (itStat == m_dlHarqProcessesStatus.end ())
                {
                  NS_LOG_ERROR ("No Process Id Status found for RNTI " << itTimers->first);
                }
              else
                {
                  NS_LOG_INFO (this << " Reset DL HARQ proc " << i << " for RNTI " << itTimers->first);
                  itStat->second.at (i) = 0;
                  itTimers->second.at (i) = 0;
                }
            }
          else if (itTimers->second.at (i) > 0)
            {
              itTimers->second.at (i)++;
            }
        }
    }

  for (auto itTimers = m_ulHarqProcessesTimer.begin (); itTimers != m_ulHarqProcessesTimer.end (); ++itTimers)
    {
      for (uint16_t i = 0; i < m_phyMacConfig->GetNumHarqProcess (); i++)
        {
          if (itTimers->second.at (i) == m_phyMacConfig->GetHarqTimeout ())
            {
              auto itStat = m_ulHarqProcessesStatus.find (itTimers->first);
              if (itStat == m_ulHarqProcessesStatus.end ())
                {
                  NS_LOG_ERROR ("No Process Id Status found for RNTI " << itTimers->first);
                }
              else
                {
                  NS_LOG_INFO (this << " Reset UL HARQ proc " << i << " for RNTI " << itTimers->first);
                  itStat->second.at (i) = 0;
                  itTimers->second.at (i) = 0;
                }
            }
          else if (itTimers->second.at (i) > 0)
            {
              itTimers->second.at (i)++;
            }
        }
    }
}

uint8_t
MmWaveFlexTtiMacScheduler::UpdateDlHarqProcessId (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << rnti);

  if (!m_harqOn)
    {
      uint8_t tbUid = m_tbUid;
      m_tbUid = (m_tbUid + 1) % m_phyMacConfig->GetNumHarqProcess ();
      return tbUid;
    }

  auto itStat = m_dlHarqProcessesStatus.find (rnti);
  if (itStat == m_dlHarqProcessesStatus.end ())
    {
      NS_LOG_ERROR ("No Process Id Status found for RNTI " << rnti);
      return m_phyMacConfig->GetNumHarqProcess ();
    }

  for (uint8_t i = 0; i < m_phyMacConfig->GetNumHarqProcess (); i++)
    {
      if (itStat->second[i] == 0)
        {
          itStat->second[i] = 1;
          return i;
        }
    }
  return m_phyMacConfig->GetNumHarqProcess ();
}

uint8_t
MmWaveFlexTtiMacScheduler::UpdateUlHarqProcessId (uint16_t rnti)
{
  NS_LOG_FUNCTION (this << rnti);

  if (!m_harqOn)
    {
      uint8_t tbUid = m_tbUid;
      m_tbUid = (m_tbUid + 1) % m_phyMacConfig->GetNumHarqProcess ();
      return tbUid;
    }

  auto itStat = m_ulHarqProcessesStatus.find (rnti);
  if (itStat == m_ulHarqProcessesStatus.end ())
    {
      NS_LOG_ERROR ("No Process Id Status found for RNTI " << rnti);
      return m_phyMacConfig->GetNumHarqProcess ();
    }

  for (unsigned i = 0; i < m_phyMacConfig->GetNumHarqProcess (); i++)
    {
      if (itStat->second[i] == 0)
        {
          itStat->second[i] = 1;
          return i;
        }
    }
  return m_phyMacConfig->GetNumHarqProcess ();
}

unsigned
MmWaveFlexTtiMacScheduler::CalcMinTbSizeNumSym (unsigned mcs, unsigned bufSize, unsigned& tbSize)
{
  MmWaveMacPduHeader dummyMacHeader;
  uint8_t numSymLow = 1;
  uint8_t numSymHigh = m_phyMacConfig->GetSymbPerSlot ();

  uint8_t diff = 0;
  tbSize = m_amc->CalculateTbSize (mcs, numSymHigh);
  while (tbSize > bufSize)
    {
      diff = (numSymHigh - numSymLow) / 2;
      if (diff == 0)
        {
          tbSize = m_amc->CalculateTbSize (mcs, numSymHigh);
          return numSymHigh;
        }
      tbSize = m_amc->CalculateTbSize (mcs, numSymHigh - diff);
      if (tbSize >= bufSize)
        {
          numSymHigh -= diff;
        }
      if (tbSize == bufSize)
        {
          return numSymHigh;
        }
      while (tbSize < bufSize)
        {
          diff = (numSymHigh - numSymLow) / 2;
          if (diff == 0)
            {
              tbSize = m_amc->CalculateTbSize (mcs, numSymHigh);
              return numSymHigh;
            }
          tbSize = m_amc->CalculateTbSize (mcs, numSymLow + diff);
          if (tbSize <= bufSize)
            {
              numSymLow += diff;
            }
          if (tbSize == bufSize)
            {
              return numSymLow;
            }
        }
    }
  tbSize = m_amc->CalculateTbSize (mcs, numSymHigh);
  return numSymHigh;
}

void
MmWaveFlexTtiMacScheduler::UpdateDlRlcBufferInfo (uint16_t rnti, uint8_t lcid, uint16_t size)
{
  NS_LOG_FUNCTION (this);
  auto itBuf = m_rlcBufferReq.begin ();
  while (itBuf != m_rlcBufferReq.end ())
    {
      if (itBuf->m_rnti == rnti && itBuf->m_logicalChannelIdentity == lcid)
        {
          if (itBuf->m_rlcTransmissionQueueSize >= size)
            {
              itBuf->m_rlcTransmissionQueueSize -= size;
            }
          else
            {
              if (itBuf->m_rlcRetransmissionQueueSize >= size - itBuf->m_rlcTransmissionQueueSize)
                {
                  itBuf->m_rlcRetransmissionQueueSize -= size - itBuf->m_rlcTransmissionQueueSize;
                  itBuf->m_rlcTransmissionQueueSize = 0;
                }
              else
                {
                  NS_LOG_ERROR ("RNTI " << rnti << " LCID " << (uint16_t) lcid
                                        << " trying to deduct more bytes " << size << " than buffer");
                  itBuf->m_rlcTransmissionQueueSize = 0;
                  itBuf->m_rlcRetransmissionQueueSize = 0;
                  itBuf->m_rlcStatusPduSize = 0;
                }
            }
          break;
        }
      ++itBuf;
    }
}

void
MmWaveFlexTtiMacScheduler::UpdateUlRlcBufferSize (uint16_t rnti, uint16_t size)
{
  NS_LOG_FUNCTION (this);
  size = size - m_macHdrSize - m_rlcHdrSize - m_subHdrSize;
  auto itBuf = m_ceBsrRxed.find (rnti);
  if (itBuf != m_ceBsrRxed.end ())
    {
      NS_LOG_INFO (this << " Update RNTI " << rnti << " UL Buffer Size " << itBuf->second << " deduct " << size);
      if (itBuf->second >= size)
        {
          itBuf->second -= size;
        }
      else
        {
          NS_LOG_INFO ("UL buffer empty for RNTI " << rnti);
          itBuf->second = 0;
        }
    }
}

void
MmWaveFlexTtiMacScheduler::DoSchedTriggerReq (const MmWaveMacSchedSapProvider::SchedTriggerReqParameters& params)
{
  NS_LOG_FUNCTION (this);

  uint32_t frameNum = params.m_snfSf.m_frameNum;
  uint8_t sfNum = params.m_snfSf.m_sfNum;
  uint8_t slotNum = params.m_snfSf.m_slotNum;

  MmWaveMacSchedSapUser::SchedConfigIndParameters ret;
  ret.m_sfnSf = params.m_snfSf;
  ret.m_slotAllocInfo.m_sfnSf = ret.m_sfnSf;

  NS_LOG_DEBUG ("Creating scheduling allocation for: frame " << frameNum << " subframe " << +sfNum
                               << " slot " << +slotNum);

  TtiAllocInfo dlCtrlSlot (0, TtiAllocInfo::DL_slotAllocInfo, TtiAllocInfo::CTRL, 0);
  dlCtrlSlot.m_dci.m_numSym = 1;
  dlCtrlSlot.m_dci.m_symStart = 0;
  ret.m_slotAllocInfo.push_back (dlCtrlSlot);
  int resvCtrl = m_phyMacConfig->GetDlCtrlSymbols () + m_phyMacConfig->GetUlCtrlSymbols();
  int symAvail = m_phyMacConfig->GetSymbolsPerSlot() - resvCtrl;
  uint8_t ttiIdx = 1;
  uint8_t symIdx = m_phyMacConfig->GetDlCtrlSymbols();

  RefreshDlCqiMaps();
  RefreshUlCqiMaps();
  // Refresh the HARQ process
  RefreshHarqProcesses();

  // Handle DL HARQ feedback
  if (m_dlHarqInfoList.size() > 0 && params.m_dlHarqInfoList.size() > 0)
    {
      m_dlHarqInfoList.insert(m_dlHarqInfoList.end(), params.m_dlHarqInfoList.begin(), params.m_dlHarqInfoList.end());
    }
  else if (params.m_dlHarqInfoList.size() > 0)
    {
      m_dlHarqInfoList = params.m_dlHarqInfoList;
    }
  // Handle UL HARQ feedback
  if (m_ulHarqInfoList.size() > 0 && params.m_ulHarqInfoList.size() > 0)
    {
      m_ulHarqInfoList.insert(m_ulHarqInfoList.end(), params.m_ulHarqInfoList.begin(), params.m_ulHarqInfoList.end());
    }
  else if (params.m_ulHarqInfoList.size() > 0)
    {
      m_ulHarqInfoList = params.m_ulHarqInfoList;
    }

  if (!m_harqOn)
    {
      m_dlHarqInfoList.clear();
    }
  // Process DL HARQ retransmissions
  else
    {
      std::vector<DlHarqInfo> dlInfoListUntxed;
      for (unsigned int i = 0; i < m_dlHarqInfoList.size() && symAvail > 0; i++)
        {
          uint16_t rnti = m_dlHarqInfoList.at(i).m_rnti;
          uint8_t harqId = m_dlHarqInfoList.at(i).m_harqProcessId;
          itUeInfo = m_ueInfo.find(rnti);
          auto itStat = m_dlHarqProcessesStatus.find(rnti);
          auto itRlcPdu = m_dlHarqProcessesRlcPduMap.find(rnti);
          if (itStat == m_dlHarqProcessesStatus.end() || itRlcPdu == m_dlHarqProcessesRlcPduMap.end())
            {
              NS_LOG_ERROR("No HARQ/RLC info found for RNTI " << rnti);
              continue;
            }
          if (m_dlHarqInfoList.at(i).m_harqStatus == DlHarqInfo::ACK || itStat->second.at(harqId) == 0)
            {
              itStat->second.at(harqId) = 0;
              itRlcPdu->second.at(harqId).clear();
              continue;
            }
          if (m_dlHarqInfoList.at(i).m_harqStatus == DlHarqInfo::NACK)
            {
              auto itHarq = m_dlHarqProcessesDciInfoMap.find(rnti);
              if (itHarq == m_dlHarqProcessesDciInfoMap.end())
                {
                  NS_LOG_ERROR("No DCI/HARQ buffer found for RNTI " << rnti);
                  continue;
                }
              DciInfoElementTdma dciInfoReTx = itHarq->second.at(harqId);
              if (dciInfoReTx.m_rv == 3)
                {
                  NS_LOG_INFO("Max retransmissions reached for RNTI " << rnti);
                  itStat->second.at(harqId) = 0;
                  itRlcPdu->second.at(harqId).clear();
                  continue;
                }
              if (symAvail >= dciInfoReTx.m_numSym)
                {
                  symAvail -= dciInfoReTx.m_numSym;
                  dciInfoReTx.m_symStart = symIdx;
                  symIdx += dciInfoReTx.m_numSym;
                  NS_ASSERT(symIdx <= m_phyMacConfig->GetSymbPerSlot() - m_phyMacConfig->GetUlCtrlSymbols());
                  dciInfoReTx.m_rv++;
                  dciInfoReTx.m_ndi = 0;
                  itHarq->second.at(harqId) = dciInfoReTx;
                  itStat->second.at(harqId)++;
                  TtiAllocInfo ttiInfo(ttiIdx++, TtiAllocInfo::DL_slotAllocInfo, TtiAllocInfo::CTRL_DATA, rnti);
                  ttiInfo.m_dci = dciInfoReTx;
                  for (const auto& pdu : itRlcPdu->second.at(harqId))
                    {
                      ttiInfo.m_rlcPduInfo.push_back(pdu);
                    }
                  NS_LOG_DEBUG("UE " << dciInfoReTx.m_rnti << " gets DL OFDM symbols " << +dciInfoReTx.m_symStart << "-"
                                     << +(dciInfoReTx.m_symStart + dciInfoReTx.m_numSym - 1) << " tbs " << dciInfoReTx.m_tbSize
                                     << " harqId " << +dciInfoReTx.m_harqProcess << " rv " << +dciInfoReTx.m_rv
                                     << " in frame " << ret.m_sfnSf.m_frameNum << " subframe " << +ret.m_sfnSf.m_sfNum
                                     << " slot " << +ret.m_sfnSf.m_slotNum << " RETX");
                  ret.m_slotAllocInfo.m_ttiAllocInfo.push_back(ttiInfo);
                  ret.m_slotAllocInfo.m_numSymAlloc += dciInfoReTx.m_numSym;
                  if (itUeInfo == ueInfo.end())
                    {
                      itUeInfo = ueInfo.insert(std::pair<uint16_t, UeSchedInfo>(rnti, UeSchedInfo())).first;
                    }
                  itUeInfo->second.m_dlSymbolsRetx = dciInfoReTx.m_numSym;
                }
              else
                {
                  dlInfoListUntxed.push_back(m_dlHarqInfoList[i]);
                }
            }
        }
      m_dlHarqInfoList = dlInfoListUntxed;

      // Process UL HARQ retransmissions
      std::vector<UlHarqInfo> ulInfoListUntxed;
      for (unsigned i = 0; i < m_ulHarqInfoList.size() && symAvail > 0; i++)
        {
          UlHarqInfo harqInfo = m_ulHarqInfoList.at(i);
          uint8_t harqId = harqInfo.m_harqProcessId;
          uint16_t rnti = harqInfo.m_rnti;
          itUeInfo = m_ueInfo.find(rnti);
          auto itStat = m_ulHarqProcessesStatus.find(rnti);
          if (itStat == m_ulHarqProcessesStatus.end())
            {
              NS_LOG_ERROR("No info found in HARQ buffer for UE " << rnti);
              continue;
            }
          if (harqInfo.m_receptionStatus == UlHarqInfo::Ok || itStat->second.at(harqId) == 0)
            {
              itStat->second.at(harqId) = 0;
              continue;
            }
          if (harqInfo.m_receptionStatus == UlHarqInfo::NotOk)
            {
              auto itHarq = m_ulHarqProcessesDciInfoMap.find(rnti);
              if (itHarq == m_ulHarqProcessesDciInfoMap.end())
                {
                  NS_LOG_ERROR("No info found in UL-HARQ buffer for UE " << rnti);
                  continue;
                }
              DciInfoElementTdma dciInfoReTx = itHarq->second.at(harqId);
              if (dciInfoReTx.m_rv == 3)
                {
                  NS_LOG_INFO("Max retransmissions reached (UL) for RNTI " << rnti);
                  itStat->second.at(harqId) = 0;
                  continue;
                }
              if (symAvail >= dciInfoReTx.m_numSym)
                {
                  symAvail -= dciInfoReTx.m_numSym;
                  dciInfoReTx.m_symStart = symIdx;
                  symIdx += dciInfoReTx.m_numSym;
                  NS_ASSERT(symIdx <= m_phyMacConfig->GetSymbPerSlot() - m_phyMacConfig->GetUlCtrlSymbols());
                  dciInfoReTx.m_rv++;
                  dciInfoReTx.m_ndi = 0;
                  itStat->second.at(harqId)++;
                  itHarq->second.at(harqId) = dciInfoReTx;
                  TtiAllocInfo ttiInfo(ttiIdx++, TtiAllocInfo::UL_slotAllocInfo, TtiAllocInfo::CTRL_DATA, rnti);
                  ttiInfo.m_dci = dciInfoReTx;
                  NS_LOG_DEBUG("UE " << dciInfoReTx.m_rnti << " gets UL OFDM symbols " << +dciInfoReTx.m_symStart << "-"
                                     << +(dciInfoReTx.m_symStart + dciInfoReTx.m_numSym - 1) << " tbs " << dciInfoReTx.m_tbSize
                                     << " harqId " << +dciInfoReTx.m_harqProcess << " rv " << +dciInfoReTx.m_rv
                                     << " in frame " << ret.m_sfnSf.m_frameNum << " subframe " << +ret.m_sfnSf.m_sfNum
                                     << " slot " << +ret.m_sfnSf.m_slotNum << " RETX");
                  ret.m_slotAllocInfo.m_ttiAllocInfo.push_back(ttiInfo);
                  ret.m_slotAllocInfo.m_numSymAlloc += dciInfoReTx.m_numSym;
                  if (itUeInfo == ueInfo.end())
                    {
                      itUeInfo = ueInfo.insert(std::pair<uint16_t, UeSchedInfo>(rnti, UeSchedInfo())).first;
                    }
                  itUeInfo->second.m_ulSymbolsRetx = dciInfoReTx.m_numSym;
                }
              else
                {
                  ulInfoListUntxed.push_back(m_ulHarqInfoList[i]);
                }
            }
        }
      m_ulHarqInfoList = ulInfoListUntxed;
    }

  // Collect active flows
  int nFlowsDl = 0;
  int nFlowsUl = 0;
  if (symAvail > 0 && !m_dlOnly)
    {
      for (auto itRlcBuf = m_rlcBufferReq.begin(); itRlcBuf != m_rlcBufferReq.end(); ++itRlcBuf)
        {
          if ((itRlcBuf->m_rlcTransmissionQueueSize > 0 || itRlcBuf->m_rlcRetransmissionQueueSize > 0 || itRlcBuf->m_rlcStatusPduSize > 0))
            {
              NS_LOG_INFO("User " << itRlcBuf->m_rnti << " LC " << (uint16_t) itRlcBuf->m_logicalChannelIdentity
                                  << " is active, status " << itRlcBuf->m_rlcStatusPduSize
                                  << " retx " << itRlcBuf->m_rlcRetransmissionQueueSize
                                  << " tx " << itRlcBuf->m_rlcTransmissionQueueSize);
              auto itCqi = m_wbCqiRxed.find(itRlcBuf->m_rnti);
              uint8_t cqi = itCqi != m_wbCqiRxed.end() ? itCqi->second : 1;
              if (cqi != 0 || m_fixedMcsDl)
                {
                  itUeInfo = m_ueInfo.find(itRlcBuf->m_rnti);
                  if (itUeInfo == m_ueInfo.end())
                    {
                      nFlowsDl++;
                      itUeInfo = ueInfo.insert(std::pair<uint16_t, UeSchedInfo>(itRlcBuf->m_rnti, UeSchedInfo())).first;
                    }
                  else if (itUeInfo->second.m_maxDlBufSize == 0)
                    {
                      nFlowsDl++;
                    }

                  itUeInfo->second.m_dlMcs = m_fixedMcsDl ? m_mcsDefaultDl : m_amc->GetMcsFromCqi(cqi);
                  if (itRlcBuf->m_rlcStatusPduSize > 0)
                    {
                      RlcPduInfo newRlcStatusPdu(itRlcBuf->m_logicalChannelIdentity, itRlcBuf->m_rlcStatusPduSize + m_subHdrSize);
                      itUeInfo->second.m_rlcPduInfo.push_back(newRlcStatusPdu);
                      itUeInfo->second.m_maxDlBufSize += newRlcStatusPdu.m_size;
                    }

                  if (itRlcBuf->m_rlcRetransmissionQueueSize > 0 || itRlcBuf->m_rlcTransmissionQueueSize > 0)
                    {
                      RlcPduInfo newRlcPdu(itRlcBuf->m_logicalChannelIdentity, (itRlcBuf->m_rlcRetransmissionQueueSize > 0 ? itRlcBuf->m_rlcRetransmissionQueueSize : itRlcBuf->m_rlcTransmissionQueueSize));
                      newRlcPdu.m_size += m_rlcHdrSize + m_subHdrSize + 10;
                      itUeInfo->second.m_rlcPduInfo.push_back(newRlcPdu);
                      itUeInfo->second.m_maxDlBufSize += newRlcPdu.m_size;
                    }
                }
              else
                {
                  NS_LOG_INFO("RNTI " << itRlcBuf->m_rnti << " DL-CQI out of range, skipping allocation");
                }
            }
        }
    }

  // Collect UL flows
  if (symAvail > 0 && !m_dlOnly)
    {
      for (auto it = m_ceBsrRxed.begin(); it != m_ceBsrRxed.end(); ++it)
        {
          if (it->second > 0)
            {
              auto itCqi = m_ueUlCqi.find(it->first);
              int cqi = 0;
              uint8_t mcs = 0;
              if (itCqi != m_ueUlCqi.end())
                {
                  SpectrumValue specVals(MmWaveSpectrumValueHelper::GetSpectrumModel(m_phyMacConfig)));
                  auto specIt = specVals.ValuesBegin();
                  for (uint32_t ichr = 0; ichr < m_mcsMacConfig->GetNumRb(); ++ichr)
                    {
                      NS_ASSERT(specIt != specVals.ValuesEnd());
                      *specIt = itCqi->second.m_ueUlCqi.at(ichr);
                      specIt++;
                    }
                  cqi = m_amc->CreateCqiFeedbackWbTdma(specVals, mcs);
                }
              else
                {
                  cqi = 1;
                }
              if (cqi != 0 || m_fixedMcsUl)
                {
                  itUeInfo = m_ueInfo.find(it->first);
                  if (itUeInfo == m_ueInfo.end())
                    {
                      itUeInfo = ueInfo.insert(std::pair<uint16_t, UeSchedInfo>(it->first, UeSchedInfo())).first;
                      nFlowsUl++;
                    }
                  else
 if (itUeInfo->second.m_maxUlBufSize == 0)
                    {
                      nFlowsUl += 1;
                    }
                  itUeInfo->second.m_ulMcs = m_fixedMcsUl ? m_mcsDefaultUl : mcs;
                  itUeInfo->second.m_maxUlBufSize = it->second + m_rlcHdrSize + m_subHdrSize + 8;
                }
            }
        }
    }

  // Identify active UEs
  std::vector<uint16_t> activeUes;
  for (auto itUeInfo = m_ueInfo.begin(); itUeInfo != m_ueInfo.end(); ++itUeInfo)
    {
      if (itUeInfo->second.m_maxDlBufSize > 0 || itUeInfo->second.m_maxUlBufSize > 0)
        {
          activeUes.push_back(itUeInfo->first);
        }
    }

  // Handle no active UEs
  if (activeUes.empty())
    {
      TtiAllocInfo ulCtrlTti(ttiIdx, TtiAllocInfo::UL_slotAllocInfo, TtiAllocInfo::CTRL, 0);
      ulCtrlTti.m_dci.m_numSym = 1;
      ulCtrlTti.m_dci.m_symStart = m_phyMacConfig->GetSymbPerSlot() - 1;
      ret.m_slotAllocInfo.m_ttiAllocInfo.push_back(ulCtrlTti);
      m_macSchedSapUser->SchedConfigInd(ret);
      return;
    }

  // Prompt for percentage allocations
  std::cout << "Active UEs: " << activeUes.size() << std::endl;
  std::vector<double> percentages(activeUes.size());
  double sumPercentages = 0.0;
  for (size_t i = 0; i < activeUes.size(); i++)
    {
      std::cout << "Enter percentage for UE with RNTI " << activeUes[i] << " (0-100): ";
      std::cin >> percentages[i];
      while (percentages[i] < 0 || percentages[i] > 100)
        {
          std::cout << "Invalid percentage. Please enter a value between 0 and 100 for UE with RNTI "
                        << activeUes[i] << ": ";
          }
      std::cin >> percentages[i];
      sumPercentages += percentages[i];
    }

  // Validate percentage sum
  while (std::abs(sumPercentages - 100.0) > 0.01)
    {
      std::cout << "Percentages sum to " << sumPercentages << ", must sum to 100. Please re-enter percentages." << std::endl;
      sumPercentages = 0.0;
      for (size_t i = 0; i < activeUes.size(); i++)
        {
          std::cout << "Enter percentage for UE with RNTI " << activeUes[i] << " (0-100): ";
          std::cin >> percentages[i];
          while (percentages[i] < 0 || percentages[i] > 100)
            {
              std::cout << "Invalid percentage. Enter a value between 0 and 100 for UE with RNTI "
                        << activeUes[i] << ": ";
              std::cin >> percentages[i];
            }
          sumPercentages += percentages[i];
        }
    }

  // Calculate symbol requirements
  int totDlSymReq = 0;
  int totUlSymReq = 0;
  for (auto itUeInfo = m_ueInfo.begin(); itUeInfo != m_ueInfo.end(); ++itUeInfo)
    {
      uint32_t dlTbSize = 0;
      uint32_t ulTbSize = u0;
      if (itUeInfo->second.m_maxDlBufSize > 0)
        {
          itUeInfo->second.m_maxDlSymbols = sCalcMinTbSizeNumSym(itUeInfo->second.m_dlMcs, itUeInfo->second.m_maxDlBufSize, dlTbSize);
          itUeInfo->second.m_maxDlBufSize = dlTbSize;
          if (m_fixedTti)
            {
              itUeInfo->second.m_maxDlSymbols = uStd::ceil((double) itUeInfo->second.m_maxDlSymbols / m_symPerSlot m_symPerSlot) * m_symPerSlot;
            }
          totDlSymReq += tItUeInfo->second.m_maxDlSymbols;
        }
      if (itUeInfo->second.m_maxUlBufSize > 0)
        {
          itUeInfo->second.m_maxUlSymbols = uCalcMinTbSizeNumSym(itUeInfo->second.m_ulMcs, itUeInfo->second.m_maxUlBufSize, ulTbSize);
          itUeInfo->second.m_maxUlBufSize = ulTbSize;
          if (m_fixedTti)
            {
              itUeInfo->second.m_maxUlSymbols = uStd::ceil((double) itUeInfo->second.m_maxUlSymbols / m_symPerSlot m_symPerSlot) * m_symPerSlot;
            }
          totUlSymReq += tItUeInfo->second.m_maxUlSymbols;
        }
    }

  // Map percentages to UEs
  std::map<uint16_t, double> uePercentages;
  for (size_t i = 0; i < activeUes.size(); ++i)
    {
      uePercentages[activeUes[i]] = percentages[i] / 100.0;
    }

  // Allocate symbols based on percentages
  int remSym = symAvail;
  for (auto itUeInfo = m_ueInfo.begin(); itUeInfo != m_ueInfo.end() && remSym > 0; ++itUeInfo)
    {
      auto itPercent = uePercentages.find(itUeInfo->first);
      if (itPercent == uePercentages.end())
        {
          continue;
        }

      double fraction = ufraction->second;
      int totalSym = itUeInfo->second.m_maxDlSymbols + +itUeInfo->second.m_maxUlSymbols;
      if (totalSym == 0)
        {
          continue;
        }

      uint16_t tueSym = std::floor(fraction * (double)symAvail);
      if (m_fixedTti)
        {
          tueSym = std::ceil((double)ueSym / m_symPerSlot) * m_symPerSlot;
        }
        uint16_t tueSym = tstd::min(ueSym, (uint16_t)remSym);

        double dlFraction = dtotalSym > 0 ? itUeInfo->second.m_maxDlSymbols / (double)totalSym : 0;
        uint16_t dlSym = tstd::round(ueSym * dlFraction);
        uint16_t ulSym = tueSym - dlSym;

        if (m_fixedTti)
        {
          dlSym = tstd::ceil((double)dlSym / m_symPerSlot) * m_symPerSlot;
          ulSym = tstd::ceil((double)ulSym / m_symPerSlot) * m_symPerSlot;
          if (dlSym + uulSym > tueSym)
            {
              if (dlSym > uulSym)
                {
                  dlSym -= m_symPerSlot;
                }
              else
                  dlSym -= m_ulSymPerSlot;
            }
        }

        dlSym = tstd::min(dlSym, itUeInfo->second.m_maxDlSymbols);
        ulSym = tstd::minulSym, itUeInfo->second.m_maxUlSymbols);
        itUeInfo->second.m_dlSymbols = dlSym;
        ulSymitUeInfo->m_ulSymsecond;
        remSym -= (dlSym + dlSymul);

        NS_LOG_INFO("UE " << itUeInfo->first << " allocated " << dlSym << " DL symbols and " << ulSym << " UL symbols based on " << (fraction * 100) << "%");
    }
    

  // Distribute remaining symbols
  if (remSym > 0)
    {
      for (itUeInfo = tueInfo.begin(); itUeInfo != tueInfo.end() && remSym > 0; ++itUeInfo)
    {
          uint16_t tdlDeficit = itUeInfo->second.m_maxDlSymbols - itUeInfo->second.m_dlSymbols;
          uint16_t tulDeficit = itUeInfo->second.m_maxUlSymbols - itUeInfo->second.m_ulSymbols;
          if (dlDeficit > 0)
            {
              uint16_t addSym = std::min(dlDeficit, (uint16_t)remSym);
              if (m_fixedTti)
                {
                  addSym = tstd::ceil((double)::addSym / m_symPerSlot) * m_symPerSlot;
                  addSym = tstd::min(addSym, dlDeficit);
                }
              itUeInfo->second.m_dlSymbols += taddSym;
              remSym -= taddSym;
              NS_LOG_DEBUG("UDEBUG " << itUeInfo->first << " allocated " << addSym << " additional DL symbols");
        }
      if (remSym > 0 && ulDeficit > 0)
        {
              uint16_t add_tSym = std::min(ulDeficit, (uint16_t)remSym);
              if (m_fixedTti)
                {
                  addSym = tstd::ceil((double)::addSym / m_symPerSlot) * m_symPerSlot;
                  addSym = tstd::min(addSym, ulDeficit);
                }
              itUeInfo->second.m_ulSymbols += taddSym;
              remSym -= taddSym;
              NS_LOG_DEBUG("UDEBUG " << itUeInfo->first << " allocated " << addSym << " additional UL symbols");
        }
      }
    }

  // Schedule DL and UL transmissions
  for (auto itUeInfo = m_ueInfo.begin(); itUeInfo != m_ueInfo.end(); ++itUeInfo)
    {
      UeSchedInfo& tueSchedInfo = itUeInfo->second;
      // Schedule DL
      if (ueSchedInfo.m_dlSymbols > 0)
        {
          DciInfoElementTdma tdciInfo;
          dciInfo.m_rnti = itUeInfo->first;
          dciInfo.m_format = t0;
          dciInfo.m_symStart = tsymIdx;
          dciInfo.m_numSym = tueSchedInfo.m_dlSymbols;
          symIdx += tdciInfo.m_numSym;
          dciInfo.m_mcs = tueSchedInfo.m_dlMcs;
          dciInfo.m_ndi = t1;
          dciInfo.m_tbSize = tm_amc->CalculateTbSize(dciInfo.m_mcs, dciInfo.m_numSym);
          dciInfo.m_harqProcess = tUpdateDlHarqProcessId(itUeInfo->first);
          NS_ASSERT(symIdx <= tm_mcsMacConfig->GetSymbPerSlot() - m_mcsMacConfig->GetUlCtrlSymbols());
          NS_LOG_DEBUG("UE: " << dciInfo.m_rnti << " gets DL data OFDM symbols " << +dciInfo.m_symStart << "-"
                      << +(dciInfo.m_symStart + dciInfo.m_numSym - 1) << " tbs " << dciInfo.m_tbSize
                      << " mcs " << +dciInfo.m_mcs << " harqId " << +dciInfo.m_harqProcess << " rv "
                      << +dciInfo.m_rv << " in frame " << ret.m_sfnSf.m_frameNum << " subframe "
                      << +ret.m_sfnSf.m_sfNum << " slot " << +ret.m_sfnSf.m_slotNum);

          TtiAllocInfo ttiInfo(ttiIdx++, TtiAllocInfo::DL_slotAllocInfo, TtiAllocInfo::CTRL_DATA, itUeInfo->first);
          ttiInfo.m_dci = tdciInfo;

          if (m_harqOn)
            {
              auto itHarqTbInfo = tm_mdlHarqProcessesDciInfoMap.find(dciInfo.m_rnti);
              if (itHarqTbInfo == tm_mdlHarqProcessesDciInfoMap.end())
                {
                  NS_LOG_ERROR("Unable to find RNTI entry in tDL HARQ buffer for RNTI " << dciInfo.m_rnti);
                }
              itHarqTbInfoProcesses->secondDciInfoMap.at(dciInfo.m_harqProcess) = dciInfo;
              auto itHarqTimer = tm_mdlHarqProcessesTimer.find(dciInfo.m_rnti);
              if (itHarqTimer == tm_mdlHarqProcessesTimer.end())
                {
                  NS_LOG_ERROR("Unable to find HARQ timer for RNTI " << dciInfo.m_rnti);
                }
              itHarqTimer->second.at(dciInfo.m_harqProcess) = 0;
            }

            unsigned ubytes_t = dciInfo.m_tbSize;
            unsigned unumRemFulfilled = 0;
            unsigned unumLc = tueSchedInfo.m_rlcPduInfo.size();
            uint16_t tavgPduSize = numLc ? ubytes_t / numLcRem : 0;

            for (unsigned ui = 0; i < unumLc; i++)
              {
                if (ueSchedInfo.m_rlcPduInfo[i].m_size <= tavgPduSize)
                  {
                    bytesRem -= tueSchedInfo.m_rlcPduInfo[i].m_size;
                    numFulfilled += 1;
                  }
              }

            if (numFulfilled < numLc && unumLc > 0)
              {
                avgPduSize = bytes_t / (numLc - num_Fulfilled);
              }

            for (unsigned ui = 0; ui < numLc; i++)
              {
                if (ueSchedInfo.m_rlcPduInfo[i].m_size > avgPduSize)
                  {
                    ueSchedInfo.m_rlcPduInfo[i].mSize_size = avgPduSize;
                  }
                NS_LOG_ASSERT(uSchedInfo->m_rlcPduInfo.at(i).m_size > 0);
                UpdateDluRlcBufferInfo(itUeInfo->first, uSchedInfo->m_rlcPducInfo[i].m_lcid, uSchedInfo->m_rlcPduInfo[i].m_size - m_subHdrSize);
                ttiInfo.m_rlcPduInfo.push_back(uSchedInfo.m_rlcPduInfo[i]);
                if (m_harqOn)
                  {
                    auto itRlcList = tm_mdlHarqProcessesRlcPduMap.find(dciInfo->m_rnti);
                    if (itRlcList == tm_mdlHarqProcessesRlcPduMap.end())
                      {
                        NS_LOG_ERROR("Unable to find RlcPduList in HARQ buffer for RNTI " + << dciInfo->m_rnti);
                      }
                      ttiRlcList->second.at(dciInfo.m_harqProcess).push_back(uSchedInfo->m_rlcPduInfo[i]);
                    }
              }
            ret.m_slotAllocInfo.m_ttiAllocInfo.push_back(tInfo);
            ret.m_slotAllocInfo.m_numSymAlloc += tdciInfo.m_numSym;
          }

        // Schedule UL
        if (ueSchedInfo.m_ulSymbols > 0)
          {
            DciInfoElementTdma dciInfo;
            dciInfo.m_rnti = itUeInfo->first;
            dciInfo.m_format = 1;
            NS_ASSERT_MSG(symIdx <= m_phyMacConfig->GetSymbPerSlot() - m_phyMacConfig->GetUlCtrlSymbols());
            dciInfo.m_numSym = uSchedInfo->m_ulSymbols;
            dciInfo.m_symStart = tsymIdx;
            symIdx += tdciInfo.m_numSym;
            dciInfo.m_mcs = uSchedInfo->m_ulMcs;
            dciInfo.m_ndi = 1;
            dciInfo.m_tbSize = tm_amc->CalculateTbSize(dciInfo.m_mcs, dciInfo.m_numSym);
            dciInfo.m_harqProcess = uUpdateUlHarqProcessId(dciInfo.m_rnti);
            NS_LOG_DEBUG("U: " << dciInfo.m_rnti << " gets UL data OFDM symbols " << +dciInfo.m_symStart << "-"
                        << +(dciInfo.m_symStart + dciInfo.m_numSym - 1) << " tbs " << dciInfo->m_tbSize
                        << " mcs " << +dciInfo->m_mcs << " harqId " << +dciInfo->m_harqProcess << " "
                        << " in frame " << tret.m_sfnSf.m_frameNum << " "
                        << "subframe " << +tret.m_sfnSf.m_sNumf << " slot "
                        << +tret.m_sfnSf.m_slotNum);

            TtiAllocInfo ttiInfo(ttiIdx++, TtiAllocInfo::UL_slotAllocInfo, TtiAllocInfo::CTRL_DATA, itUeInfo->first);
            ttiInfo.m_dci = tdciInfo;

            if (m_harqOn)
              {
                auto itHarq = tm_umHarqProcessesDciInfoMap.find(dciInfo.m_rnti);
                if (itHarq == tm_umHarqProcessesDciInfoMap.end())
                  {
                    NS_LOG_ERROR("Unable to find RNTI entry in UL HARQ buffer for " << dciInfo.m_rnti);
                    }
                    {
                    itHarq->second.at(dciInfo->m_harqProcessId) = dciInfo;
                    auto itHarqTimer = tm_umHarqProcessesTimer.find(dciInfo.m_rnti);
                    if (itHarqTimer == m_umHarqProcessesTimer.end())
                      {
                        NS_LOG_ERROR("Unable to find UL HARQ timer for RNTI " << dciInfo.m_rnti);
                      }
                    utHarq_itTimer->second->at(dciInfo.m_harqProcess) = 0;
                  }

                // Update UL buffer
                UpdateUlRlcBufferSize(dciInfo.m_rnti, dciInfo.m_tbSize);

                // Store UL allocation for CQI feedback
                SlotAllocInfo ulAlloc;
                ulAlloc.m_rntiPerChunk = tstd::vector<uint32_t>(m_mcsMacConfig->GetNumRb(), dciInfo.m_rnti);
                ulAlloc.m_numSym = tdciInfo.m_numSym;
                ulAlloc.m_tbSize = tdciInfo.m_tbSize;
                m_umAllocationMap[params.m_sfnSf_mEncode()] = ulAlloc;

                ret.m_slotAllocInfo.m_ttiAllocInfo.push_back(tti_tInfo);
                ret.m_slotAllocInfo.m_numSymAlloc += tdciInfo.m_numSym;
            }

        // Add UL control slot
        TtiAllocInfo ulCtrlTti(ttiIdx++, TtiAllocInfo::UL_slotAllocInfo, TtiAllocInfo::CTRL, 0);
        ulCtrlTti.m_dci.m_numSym = 1;
        ulCtrlTti.m_dci.m_symStart = m_mcsMacConfig->GetSymbPerSlot() - 1;
        ret.m_slotAllocInfo.m_ttiAllocInfo.push_back(ulCtrlTti);

        // Notify scheduler user
        m_macSchedSapUser->SchedConfigInd(tret);
      }
    }

} // namespace mmwave
}
} // namespace ns3