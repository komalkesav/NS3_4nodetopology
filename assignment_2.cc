#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/config-store.h"
#include "ns3/spectrum-module.h"
#include "ns3/applications-module.h"
#include <ns3/buildings-helper.h>
#include "ns3/point-to-point-helper.h"
#include <cmath>
#include "ns3/internet-module.h"

using namespace ns3;

int main(int argc, char *argv[])
{
  Config::SetDefault("ns3::LteEnbPhy::TxPower", DoubleValue(40.0));
  double enbDist = 5000.0;
  double radius = 500.0;
  uint32_t numUes = 10;
  double simTime = 30.0;
  char scheduler[] = "ns3::PfFfMacScheduler";
  UintegerValue runValue;
  uint32_t numEnB = 4;
  double speed = 0.0;

  CommandLine cmd(__FILE__);
  cmd.AddValue("scheduler", "the scheduler to use [ns3::PfFfMacScheduler]", scheduler);
  cmd.AddValue("speed", "speed with which ue's are moving [10.0]", speed);
  cmd.AddValue("simTime", "simulation run time [30.0]", simTime);
  cmd.Parse(argc, argv);

  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults();
  cmd.Parse(argc, argv);

  // checking the Random Generator
  GlobalValue::GetValueByName("RngRun", runValue);

  // setting the speed
  Ptr<ns3::ConstantRandomVariable> x = CreateObject<ns3::ConstantRandomVariable>();
  x->SetAttribute("Constant", ns3::DoubleValue(speed));

  // Create Nodes: eNodeB and UE
  NodeContainer enbNodes;
  NodeContainer ueNodes1;
  NodeContainer ueNodes2;
  NodeContainer ueNodes3;
  NodeContainer ueNodes4;
  enbNodes.Create(numEnB);
  ueNodes1.Create(numUes);
  ueNodes2.Create(numUes);
  ueNodes3.Create(numUes);
  ueNodes4.Create(numUes);

  // lte Helper and EPC helper
  Ptr<ns3::LteHelper> lteHelper = CreateObject<ns3::LteHelper>();
  lteHelper->SetSchedulerType(scheduler);
  lteHelper->SetHandoverAlgorithmType("ns3::A3RsrpHandoverAlgorithm");
  lteHelper->SetEnbDeviceAttribute("UlBandwidth", UintegerValue(50));
  lteHelper->SetEnbDeviceAttribute("DlBandwidth", UintegerValue(50));
  std::cout << "RngRun:" << runValue.Get() << std::endl;

  // Position of eNBs
  Ptr<ListPositionAllocator> PositionAlloc = CreateObject<ListPositionAllocator>();
  PositionAlloc->Add(Vector(0.0, 0.0, 0.0));
  PositionAlloc->Add(Vector(enbDist, 0.0, 0.0));
  PositionAlloc->Add(Vector(0.0, enbDist, 0.0));
  PositionAlloc->Add(Vector(enbDist, enbDist, 0.0));
  MobilityHelper enbMobility;
  enbMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  enbMobility.SetPositionAllocator(PositionAlloc);
  enbMobility.Install(enbNodes);

  // Position of UEs for eNB 1
  MobilityHelper uemobility;
  uemobility.SetPositionAllocator("ns3::UniformDiscPositionAllocator", "X", DoubleValue(0.0), "Y", DoubleValue(0.0), "rho", DoubleValue(radius));
  uemobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel", "Mode", EnumValue(ns3::RandomWalk2dMobilityModel::Mode::MODE_TIME), "Bounds", RectangleValue(Rectangle(-1000.0, 6000.0, -1000.0, 6000.0)), "Time", TimeValue(Time("5s")), "Speed", PointerValue(x));
  uemobility.Install(ueNodes1);

  // // Position of UEs for eNB 2
  uemobility.SetPositionAllocator("ns3::UniformDiscPositionAllocator", "X", DoubleValue(enbDist), "Y", DoubleValue(0.0), "rho", DoubleValue(radius));
  uemobility.Install(ueNodes2);

  // // Position of UEs for eNB 3
  uemobility.SetPositionAllocator("ns3::UniformDiscPositionAllocator", "X", DoubleValue(0.0), "Y", DoubleValue(enbDist), "rho", DoubleValue(radius));
  uemobility.Install(ueNodes3);

  // Position of UEs for eNB 4
  uemobility.SetPositionAllocator("ns3::UniformDiscPositionAllocator", "X", DoubleValue(enbDist), "Y", DoubleValue(enbDist), "rho", DoubleValue(radius));
  uemobility.Install(ueNodes4);

  // // Attach a trace to print postion whenever Course Changes

  // Create Devices and install them in the Nodes (eNB and UE)
  NetDeviceContainer enbDevs = lteHelper->InstallEnbDevice(enbNodes);
  NetDeviceContainer ueDevs1 = lteHelper->InstallUeDevice(ueNodes1);
  NetDeviceContainer ueDevs2 = lteHelper->InstallUeDevice(ueNodes2);
  NetDeviceContainer ueDevs3 = lteHelper->InstallUeDevice(ueNodes3);
  NetDeviceContainer ueDevs4 = lteHelper->InstallUeDevice(ueNodes4);

  // Attach UEs to a eNB
  lteHelper->AttachToClosestEnb(ueDevs1, enbDevs);
  lteHelper->AttachToClosestEnb(ueDevs2, enbDevs);
  lteHelper->AttachToClosestEnb(ueDevs3, enbDevs);
  lteHelper->AttachToClosestEnb(ueDevs4, enbDevs);

  // Activate an EPS bearer
  EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
  EpsBearer bearer(q);
  lteHelper->ActivateDataRadioBearer(ueDevs1, bearer);
  lteHelper->ActivateDataRadioBearer(ueDevs2, bearer);
  lteHelper->ActivateDataRadioBearer(ueDevs3, bearer);
  lteHelper->ActivateDataRadioBearer(ueDevs4, bearer);

  Simulator::Stop(Seconds(simTime));

  // REM Plotter code USE when only plotting REM map and not for simulation.
  Ptr<RadioEnvironmentMapHelper> remHelper = CreateObject<RadioEnvironmentMapHelper>();
  remHelper->SetAttribute("Channel", PointerValue(lteHelper->GetDownlinkSpectrumChannel()));
  remHelper->SetAttribute("OutputFile", StringValue("rem.out"));
  remHelper->SetAttribute("XMin", DoubleValue(-2000.0));
  remHelper->SetAttribute("XMax", DoubleValue(7000.0));
  remHelper->SetAttribute("XRes", UintegerValue(100));
  remHelper->SetAttribute("YMin", DoubleValue(-2000.0));
  remHelper->SetAttribute("YMax", DoubleValue(7000.0));
  remHelper->SetAttribute("YRes", UintegerValue(75));
  remHelper->SetAttribute("Z", DoubleValue(0.0));
  remHelper->Install();

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}