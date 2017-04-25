/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 */

/* Network topology
 * n0-----bs1-----bs2-----n1
 *   ^^^	  ^^^	  ^^^
 * wireless   p2p    wireless
 * TCP flow form n0 to n1
 * bs1 and bs2 are base stations.
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/flow-monitor-module.h"

NS_LOG_COMPONENT_DEFINE ("wifi-tcp");

using namespace ns3;

Ptr<PacketSink> sink;                               /* Pointer to the packet sink application */
uint64_t lastTotalRx = 0;                           /* The value of the last total received bytes */


void solve(int argc, char *argv[],int a);

int
main (int argc, char *argv[])
{
	std::cout<<"+--------------------------------------------+\n";
	std::string message = "WIRELESS TOPOLOGY EXPERIMENT";
	std::cout<<"\t" << message << "\t\n";
	std::cout<<"+--------------------------------------------+\n";
	int payloadSize[]={40, 44, 48, 52, 60, 552, 576, 628, 1420,1500};
	for (int i = 0; i < 10 ; ++i)
	{
		std::cout<<"PACKETSIZE: "<< payloadSize[i] <<" BYTES" << std::endl;
		solve(argc,argv,payloadSize[i]);
	}
}


void solve(int argc, char *argv[],int payloadSize)
{

    std::string lat = "100ms";
    std::string rate = "10Mbps";                       /* P2P link speed */
    // uint32_t payloadSize = 1472;                       /* Transport layer payload size in bytes. */
    std::string dataRate = "100Mbps";                  /* Application layer datarate. */
    std::string tcpVariant = "ns3::TcpReno";           /* TCP variant type. */
    std::string phyRate = "HtMcs7";                    /* Physical layer bitrate. */
    bool pcapTracing = false;                          /* PCAP Tracing is enabled or not. */

    /* Command line argument parser setup. */
    CommandLine cmd;
    cmd.AddValue ("payloadSize", "Payload size in bytes", payloadSize);
    cmd.AddValue ("dataRate", "Application data ate", dataRate);
    cmd.AddValue ("tcpVariant", "Transport protocol to use: TcpTahoe, TcpReno, TcpNewReno, TcpWestwood, 			  TcpWestwoodPlus ", tcpVariant);
    cmd.AddValue ("phyRate", "Physical layer bitrate", phyRate);
    cmd.AddValue ("pcap", "Enable/disable PCAP Tracing", pcapTracing);
    // cmd.AddValue ("simulationTime", "Simulation time in seconds", simulationTime);
    cmd.Parse (argc, argv);

    /* No fragmentation and no RTS/CTS */
    Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("999999"));
    Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("999999"));

    /* Configure TCP Options */
    Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));

    WifiMacHelper wifiMac;
    WifiHelper wifiHelper;
    wifiHelper.SetStandard (WIFI_PHY_STANDARD_80211n_5GHZ);

    /* Set up Legacy Channel */
    YansWifiChannelHelper wifiChannel,wifiChannel2;
    wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue (5e9));
    wifiChannel2.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel2.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue (5e9));

    /* Setup Physical Layer for first interface*/
    YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
    wifiPhy.SetChannel (wifiChannel.Create ());
    wifiPhy.Set ("TxPowerStart", DoubleValue (10.0));
    wifiPhy.Set ("TxPowerEnd", DoubleValue (10.0));
    wifiPhy.Set ("TxPowerLevels", UintegerValue (1));
    wifiPhy.Set ("TxGain", DoubleValue (0));
    wifiPhy.Set ("RxGain", DoubleValue (0));
    wifiPhy.Set ("RxNoiseFigure", DoubleValue (10));
    wifiPhy.Set ("CcaMode1Threshold", DoubleValue (-79));
    wifiPhy.Set ("EnergyDetectionThreshold", DoubleValue (-79 + 3));
    wifiPhy.SetErrorRateModel ("ns3::YansErrorRateModel");
    wifiHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                        "DataMode", StringValue (phyRate),
                                        "ControlMode", StringValue ("HtMcs0"));

    /* Setup Physical Layer for second interface*/
    YansWifiPhyHelper wifiPhy2 = YansWifiPhyHelper::Default ();
    wifiPhy2.SetChannel (wifiChannel.Create ());
    wifiPhy2.Set ("TxPowerStart", DoubleValue (10.0));
    wifiPhy2.Set ("TxPowerEnd", DoubleValue (10.0));
    wifiPhy2.Set ("TxPowerLevels", UintegerValue (1));
    wifiPhy2.Set ("TxGain", DoubleValue (0));
    wifiPhy2.Set ("RxGain", DoubleValue (0));
    wifiPhy2.Set ("RxNoiseFigure", DoubleValue (10));
    wifiPhy2.Set ("CcaMode1Threshold", DoubleValue (-79));
    wifiPhy2.Set ("EnergyDetectionThreshold", DoubleValue (-79 + 3));
    wifiPhy2.SetErrorRateModel ("ns3::YansErrorRateModel");
    wifiHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                        "DataMode", StringValue (phyRate),
                                        "ControlMode", StringValue ("HtMcs0"));

    /* All the nodes in the network */
    NodeContainer networkNodes;
    networkNodes.Create (4);
    Ptr<Node> staWifiNode = networkNodes.Get (0);
    Ptr<Node> apWifiNode = networkNodes.Get (1);
    Ptr<Node> apWifiNode2 = networkNodes.Get (2);
    Ptr<Node> staWifiNode2 = networkNodes.Get (3);

    /* For the point to point link*/
    NodeContainer bs1bs2 = NodeContainer (networkNodes.Get (1), networkNodes.Get (2));

    /* Configure access points */
    Ssid ssid = Ssid ("network");
    wifiMac.SetType ("ns3::ApWifiMac",
                     "Ssid", SsidValue (ssid));

    NetDeviceContainer apDevice;
    NetDeviceContainer apDevice2;
    apDevice = wifiHelper.Install (wifiPhy, wifiMac, apWifiNode);
    apDevice2 = wifiHelper.Install (wifiPhy2, wifiMac, apWifiNode2);

    /* Configure Stations */
    wifiMac.SetType ("ns3::StaWifiMac",
                     "Ssid", SsidValue (ssid));

    NetDeviceContainer staDevices;
    NetDeviceContainer staDevices2;
    staDevices = wifiHelper.Install (wifiPhy, wifiMac, staWifiNode);
    staDevices2 = wifiHelper.Install (wifiPhy2, wifiMac, staWifiNode2);

    /* bs1-bs2 point to point connection*/
    PointToPointHelper p2p;
    p2p.SetChannelAttribute ("Delay", StringValue (lat));
    p2p.SetDeviceAttribute ("DataRate", StringValue (rate));
    NetDeviceContainer d1d2 = p2p.Install (bs1bs2);


    /* Mobility model */
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
    positionAlloc->Add (Vector (0.0, 0.0, 0.0));
    positionAlloc->Add (Vector (1.0, 0.0, 0.0));
    positionAlloc->Add (Vector (2.0, 0.0, 0.0));
    positionAlloc->Add (Vector (3.0, 0.0, 0.0));

    mobility.SetPositionAllocator (positionAlloc);
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (staWifiNode);
    mobility.Install (apWifiNode);
    mobility.Install (apWifiNode2);
    mobility.Install (staWifiNode2);

    /* Internet stack */
    InternetStackHelper stack;
    stack.Install (networkNodes);

    Ipv4AddressHelper address,address2,pointaddress;
    address.SetBase ("10.1.1.0", "255.255.255.0");
    pointaddress.SetBase ("10.1.2.0", "255.255.255.0");
    address2.SetBase ("10.1.3.0", "255.255.255.0");

    Ipv4InterfaceContainer apInterface,apInterface2;
    Ipv4InterfaceContainer staInterface,staInterface2;

    /* Addresses assigned consecutively */
    staInterface = address.Assign (staDevices);
    apInterface = address.Assign (apDevice);
    apInterface2 = address2.Assign (apDevice2);
    staInterface2 = address2.Assign (staDevices2);

    Ipv4InterfaceContainer b1b2 = pointaddress.Assign (d1d2);

    /* Populate routing table */
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    uint16_t sinkPort[] = {11,12,13,14,15};
    int totalPorts=sizeof(sinkPort)/sizeof(sinkPort[0]);
    for (int i = 0; i < totalPorts; ++i)
    {
	    /* Install TCP Receiver on the sink */
	    PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort[i]));
	    ApplicationContainer sinkApp = sinkHelper.Install (staWifiNode2);
	    sink = StaticCast<PacketSink> (sinkApp.Get(0));

		/* Install TCP/UDP Transmitter on the station */
	    OnOffHelper server ("ns3::TcpSocketFactory", (InetSocketAddress (staInterface2.GetAddress (0), sinkPort[i])));
	    server.SetAttribute ("PacketSize", UintegerValue (payloadSize));
	    server.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
	    server.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
	    server.SetAttribute ("DataRate", DataRateValue (DataRate (dataRate)));
	    ApplicationContainer serverApp = server.Install (staWifiNode);

	    /* Start Applications */
	    sinkApp.Start (Seconds (0.0));
	    sinkApp.Stop (Seconds (10.));
	    serverApp.Start (Seconds (1.0));
	    serverApp.Stop (Seconds (10.0));
    }

    /* Flow monitor */
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();
    NS_LOG_INFO ("Run Simulation.");
    Simulator::Stop (Seconds(5.0));
    Simulator::Run ();

    monitor->CheckForLostPackets ();
    int totalSumOfThroughput=0;
    int totalSquareSumThroughPut = 0;
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
      if ((t.sourceAddress=="10.1.1.1" && t.destinationAddress == "10.1.3.2"))
      {
        std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
        std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
        std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
        int currentThrougput =  i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024;
        std::cout << "  Throughput: " << currentThrougput << " Kbps\n";
        totalSumOfThroughput += currentThrougput;
        totalSquareSumThroughPut += currentThrougput * currentThrougput;
      }
  	}
    std::string first= "+----------";
    std::string middle= "----------------";
    std::string last= "---------+";
    //std::cout<<totalSquareSumThroughPut << " "<< totalSumOfThroughput << std::endl;
    std::cout<<first+middle+last<<std::endl;
    float JainIndex = (totalSumOfThroughput*totalSumOfThroughput)/((float)totalPorts*totalSquareSumThroughPut);
    std::cout<<"Jain's Fairness Index: "<<JainIndex<<std::endl;
    monitor->SerializeToXmlFile("lab-1.flowmon", true, true);
    std::cout<<first<<middle+middle+middle<<last<<"\n" <<std::endl;

    Simulator::Destroy ();
    NS_LOG_INFO ("Done.");

    // return 0;

}