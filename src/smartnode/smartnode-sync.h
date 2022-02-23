// Copyright (c) 2014-2019 The Dash Core developers
// Copyright (c) 2020 The Raptoreum developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef SMARTNODE_SYNC_H
#define SMARTNODE_SYNC_H

#include <atomic>
#include "chain.h"
#include "net.h"

class CSmartnodeSync;

static const int SMARTNODE_SYNC_FAILED          = -1;
static const int SMARTNODE_SYNC_INITIAL         = 0; // sync just started, was reset recently or still in IDB
static const int SMARTNODE_SYNC_WAITING         = 1; // waiting after initial to see if we can get more headers/blocks
static const int SMARTNODE_SYNC_GOVERNANCE      = 4;
static const int SMARTNODE_SYNC_GOVOBJ          = 10;
static const int SMARTNODE_SYNC_GOVOBJ_VOTE     = 11;
static const int SMARTNODE_SYNC_FINISHED        = 999;

static const int SMARTNODE_SYNC_TICK_SECONDS    = 6;
static const int SMARTNODE_SYNC_TIMEOUT_SECONDS = 30; // our blocks are 2.5 minutes so 30 seconds should be fine

extern CSmartnodeSync smartnodeSync;

//
// CSmartnodeSync : Sync smartnode assets in stages
//

class CSmartnodeSync
{
private:
    // Keep track of current asset
    std::atomic<int> nCurrentAsset{SMARTNODE_SYNC_WAITING};
    // Count peers we've requested the asset from
    std::atomic<int> nTriedPeerCount{0};

    // Time when current smartnode asset sync started
    std::atomic<int64_t> nTimeAssetSyncStarted{0};
    // ... last bumped
    std::atomic<int64_t> nTimeLastBumped{0};
    // ... or failed
    std::atomic<int64_t> nTimeLastFailure{0};

public:
    CSmartnodeSync() { Reset(); }


    void SendGovernanceSyncRequest(CNode* pnode, CConnman& connman);

    bool IsFailed() { return nCurrentAsset == SMARTNODE_SYNC_FAILED; }
    bool IsBlockchainSynced() { return nCurrentAsset > SMARTNODE_SYNC_WAITING; }
    bool IsSynced() { return nCurrentAsset == SMARTNODE_SYNC_FINISHED; }

    int GetAssetID() { return nCurrentAsset; }
    int GetAttempt() { return nTriedPeerCount; }
    void BumpAssetLastTime(const std::string& strFuncName);
    int64_t GetAssetStartTime() { return nTimeAssetSyncStarted; }
    std::string GetAssetName();
    std::string GetSyncStatus();

    void Reset();
    void SwitchToNextAsset(CConnman& connman);

    void ProcessMessage(CNode* pfrom, const std::string& strCommand, CDataStream& vRecv);
    void ProcessTick(CConnman& connman);

    void AcceptedBlockHeader(const CBlockIndex *pindexNew);
    void NotifyHeaderTip(const CBlockIndex *pindexNew, bool fInitialDownload, CConnman& connman);
    void UpdatedBlockTip(const CBlockIndex *pindexNew, bool fInitialDownload, CConnman& connman);

    void DoMaintenance(CConnman &connman);
};

#endif
