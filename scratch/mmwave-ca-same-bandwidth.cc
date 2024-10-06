/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/applications-module.h"
#include "ns3/config-store.h"
#include "ns3/core-module.h"
#include "ns3/global-route-manager.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/isotropic-antenna-model.h"
#include "ns3/log.h"
#include "ns3/mmwave-helper.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include <ns3/buildings-helper.h>

#include <map>
#include <string>
#include <iostream>

using namespace ns3;
using namespace mmwave;

/* In this example a single UE is connected with a single MmWave BS. The UE is
 * placed at distance ueDist from the BS and it does not move. The system
 * bandwidth is fixed at 1GHz. If CA is enabled, 2 CCs are used and each of them
 * uses half of the total bandwidth.
 */

int
main(int argc, char* argv[])
{
    bool useCa = false;
    double ueDist = 1500;
    bool blockage = true;
    double totalBandwidth = 200e6;
    double frequency0 = 3.5e9;
    double frequency1 = 3.7e9;
    double simTime = 20;
    double txPower = 10;
    double ueNum = 10;
    double ueSpeed = 9.7;  // Speed in meters per second (e.g., walking: 1.4m/s, urban driving: 9.7m/s, highway driving: 22.2m/s)

    std::string condition =
        "l"; // channel condition, l = LOS, n = NLOS, otherwise the condition is randomly determined

    CommandLine cmd;
    cmd.AddValue("useCa", "If enabled use 2 CC", useCa);
    cmd.AddValue("ueDist", "Distance between Enb and Ue [m]", ueDist);
    cmd.AddValue("blockage", "If enabled blockage = true", blockage);
    cmd.AddValue("frequency0", "CC 0 central frequency", frequency0);
    cmd.AddValue("frequency1", "CC 1 central frequency", frequency1);
    cmd.AddValue("totalBandwidth", "System bandwidth in Hz", totalBandwidth);
    cmd.AddValue("simTime", "Simulation time", simTime);
    cmd.AddValue(
        "condition",
        "Channel condition, l = LOS, n = NLOS, otherwise the condition is randomly determined",
        condition);
    cmd.Parse(argc, argv);

    // CC 0
    // 1. create MmWavePhyMacCommon object
    Ptr<MmWavePhyMacCommon> phyMacConfig0 = CreateObject<MmWavePhyMacCommon>();
    phyMacConfig0->SetBandwidth(totalBandwidth / 2);
    phyMacConfig0->SetNumerology(MmWavePhyMacCommon::Numerology::NrNumerology2);
    phyMacConfig0->SetCentreFrequency(frequency0);

    // 2. create the MmWaveComponentCarrier object
    Ptr<MmWaveComponentCarrier> cc0 = CreateObject<MmWaveComponentCarrier>();
    cc0->SetConfigurationParameters(phyMacConfig0);
    cc0->SetAsPrimary(true);

    // CC 1
    Ptr<MmWaveComponentCarrier> cc1;
    if (useCa)
    {
        // 1. create MmWavePhyMacCommon object
        Ptr<MmWavePhyMacCommon> phyMacConfig1 = CreateObject<MmWavePhyMacCommon>();
        phyMacConfig1->SetBandwidth(totalBandwidth / 2);
        phyMacConfig1->SetCentreFrequency(frequency1);
        phyMacConfig1->SetCcId(1);

        // 2. create the MmWaveComponentCarrier object
        cc1 = CreateObject<MmWaveComponentCarrier>();
        cc1->SetConfigurationParameters(phyMacConfig1);
        cc1->SetAsPrimary(false);
    }

    // create the CC map
    std::map<uint8_t, MmWaveComponentCarrier> ccMap;
    ccMap[0] = *cc0;
    if (useCa)
    {
        ccMap[1] = *cc1;
    }

    // print CC parameters
    for (uint8_t i = 0; i < ccMap.size(); i++)
    {
        std::cout << "Component Carrier "
                  << (uint32_t)(ccMap[i].GetConfigurationParameters()->GetCcId()) << " frequency : "
                  << ccMap[i].GetConfigurationParameters()->GetCenterFrequency() / 1e9 << " GHz,"
                  << " bandwidth : " << totalBandwidth / 2 / 1e6 << " MHz," << std::endl;
    }

    // create and set the helper
    // first set UseCa = true, then NumberOfComponentCarriers
    Config::SetDefault("ns3::MmWaveHelper::UseCa", BooleanValue(useCa));
    if (useCa)
    {
        Config::SetDefault("ns3::MmWaveHelper::NumberOfComponentCarriers", UintegerValue(2));
        Config::SetDefault("ns3::MmWaveHelper::EnbComponentCarrierManager",
                           StringValue("ns3::MmWaveRrComponentCarrierManager"));
    }

    Config::SetDefault("ns3::MmWaveEnbPhy::TxPower", DoubleValue(txPower));
    Config::SetDefault("ns3::MmWaveHelper::ChannelModel",
                       StringValue("ns3::ThreeGppSpectrumPropagationLossModel"));
    Config::SetDefault("ns3::ThreeGppChannelModel::Scenario", StringValue("UMa"));
    Config::SetDefault("ns3::ThreeGppChannelModel::Blockage",
                       BooleanValue(blockage)); // Enable/disable the blockage model
    Config::SetDefault("ns3::MmWaveHelper::PathlossModel",
                       StringValue("ns3::ThreeGppUmaPropagationLossModel"));

    // by default, isotropic antennas are used. To use the 3GPP radiation pattern instead, use the
    // <ThreeGppAntennaArrayModel> beware: proper configuration of the bearing and downtilt angles
    // is needed
    Config::SetDefault("ns3::PhasedArrayModel::AntennaElement",
                       PointerValue(CreateObject<IsotropicAntennaModel>()));

    Ptr<MmWaveHelper> helper = CreateObject<MmWaveHelper>();
    helper->SetCcPhyParams(ccMap);
    if (condition == "l")
    {
        helper->SetChannelConditionModelType("ns3::AlwaysLosChannelConditionModel");
    }
    else if (condition == "n")
    {
        helper->SetChannelConditionModelType("ns3::NeverLosChannelConditionModel");
    }

    // create the enb node
    NodeContainer enbNodes;
    enbNodes.Create(1);

    // set mobility
    Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator>();
    enbPositionAlloc->Add(Vector(0.0, 0.0, 15.0));

    MobilityHelper enbmobility;
    enbmobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    enbmobility.SetPositionAllocator(enbPositionAlloc);
    enbmobility.Install(enbNodes);
    BuildingsHelper::Install(enbNodes);

    // install enb device
    NetDeviceContainer enbNetDevices = helper->InstallEnbDevice(enbNodes);
    std::cout << "eNB device installed" << std::endl;

    // create ue node
    NodeContainer ueNodes;
    ueNodes.Create(ueNum);  // Create 10 UEs

    // Set mobility with specific positions and velocity for each UE
    MobilityHelper uemobility;
    Ptr<ListPositionAllocator> uePositionAlloc = CreateObject<ListPositionAllocator>();

    // Set UE positions with a difference of 100 along the x-axis (1000, 1100, ..., 1900) and y-axis is 0
    for (uint32_t i = 0; i < ueNum; ++i) {
        double xPosition = 500 + 50 * i;  // x position for each UE
        uePositionAlloc->Add(Vector(xPosition, 0.0, 1.6));  // z-axis is height
    }

    uemobility.SetPositionAllocator(uePositionAlloc);

    // Set RandomWalk2dMobilityModel with specified velocity (example: 2m/s to 4m/s)
    uemobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                                "Bounds", RectangleValue(Rectangle(-2000, 2000, -2000, 2000)),
                                "Speed", StringValue("ns3::ConstantRandomVariable[Constant=" + std::to_string(ueSpeed) + "]"),
                                "Distance", DoubleValue(50.0),  // Optional distance between changes
                                "Direction", StringValue("ns3::UniformRandomVariable[Min=0|Max=6.283185]"));
    uemobility.Install(ueNodes);
    BuildingsHelper::Install(ueNodes);

    std::cout << "UE initial positions: " << std::endl;
    for (uint32_t i = 0; i < ueNodes.GetN(); ++i) {
        Ptr<MobilityModel> mobility = ueNodes.Get(i)->GetObject<MobilityModel>();
        Vector pos = mobility->GetPosition();
        std::cout << "UE " << i << " position: (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
    }


    /*
    // set mobility
    std::cout << "Distance : " << (uint32_t)ueDist << " m" << std::endl;
    MobilityHelper uemobility;
    uemobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    Ptr<ListPositionAllocator> uePositionAlloc = CreateObject<ListPositionAllocator>();
    uePositionAlloc->Add(Vector(ueDist, 0.0, 1.6));
    uemobility.SetPositionAllocator(uePositionAlloc);
    uemobility.Install(ueNodes.Get(0));
    BuildingsHelper::Install(ueNodes);
    */

    // install ue device
    NetDeviceContainer ueNetDevices = helper->InstallUeDevice(ueNodes);
    std::cout << "UE device installed" << std::endl;

    helper->AttachToClosestEnb(ueNetDevices, enbNetDevices);
    helper->EnableTraces();

    // Mobility tracing
    AsciiTraceHelper ascii;
    MobilityHelper::EnableAsciiAll(ascii.CreateFileStream("mobility-trace-example.mob"));


    // activate a data radio bearer
    enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
    EpsBearer bearer(q);
    helper->ActivateDataRadioBearer(ueNetDevices, bearer);

    Simulator::Stop(Seconds(simTime));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
