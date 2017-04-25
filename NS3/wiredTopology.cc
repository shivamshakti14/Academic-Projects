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
 * n2-----r1-----r2-----n3
 * TCP flow form n2 to n3
 * r1 and r2 are routers.
 */

#include <fstream>
#include <string>
#include <string>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/ipv4-global-routing-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Lab2");

class MyApp : public Application
{
public:

  MyApp ();
  virtual ~MyApp();

  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);
  void ChangeRate(DataRate newrate);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
};

MyApp::MyApp ()
  : m_socket (0),
	m_peer (),
	m_packetSize (0),
	m_nPackets (0),
	m_dataRate (0),
	m_sendEvent (),
	m_running (false),
	m_packetsSent (0)
{
}

MyApp::~MyApp()
{
  m_socket = 0;
}

void
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
}

void
MyApp::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
  m_socket->Bind ();
  m_socket->Connect (m_peer);
  SendPacket ();
}

void
MyApp::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
	{
	  Simulator::Cancel (m_sendEvent);
	}

  if (m_socket)
	{
	  m_socket->Close ();
	}
}

void
MyApp::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);

  if (++m_packetsSent < m_nPackets)
	{
	  ScheduleTx ();
	}
}

void
MyApp::ScheduleTx (void)
{
  if (m_running)
	{
	  Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
	  m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
	}
}

void
MyApp::ChangeRate(DataRate newrate)
{
   m_dataRate = newrate;
   return;
}

static void
CwndChange (uint32_t oldCwnd, uint32_t newCwnd)
{
  // std::cout << Simulator::Now ().GetSeconds () << "\t" << newCwnd <<"\n";
}

void
IncRate (Ptr<MyApp> app, DataRate rate)
{
	app->ChangeRate(rate);
	return;
}

void solve(int argc, char *argv[],int a);

int main (int argc, char *argv[])
{
	std::cout<<"+--------------------------------------------+\n";
	std::string message = "WIRED TOPOLOGY EXPERIMENT";
	std::cout<<"\t" << message << "\t\n";
	std::cout<<"+--------------------------------------------+\n";
	int packetSize[]={40, 44, 48, 52, 60, 552, 576, 628, 1420,1500};
	for (int i = 0; i < 10 ; ++i)
	{
		std::cout<<"PACKETSIZE: "<< packetSize[i] <<" BYTES" << std::endl;
		solve(argc,argv,packetSize[i]);
	}
}


void solve(int argc, char *argv[],int packetSize)
{

	std::string lat = "20ms";
	std::string rate = "100Mbps"; // P2P link
	// int packetSize=1040;
	bool enableFlowMonitor = false;
	std::string tcpVariant = "ns3::TcpReno";        /* TCP variant type. */

	// std::cout<<"Enter the packet Size: ";
	// std::cin>>packetSize;

	CommandLine cmd;
	cmd.AddValue ("latency", "P2P link Latency in miliseconds", lat);
	cmd.AddValue ("rate", "P2P data rate in bps", rate);
	cmd.AddValue ("EnableMonitor", "Enable Flow Monitor", enableFlowMonitor);
	cmd.AddValue ("tcpVariant", "Transport protocol to use: TcpTahoe, TcpReno, TcpNewReno, TcpWestwood, TcpWestwoodPlus ", tcpVariant);
	// cmd.AddValue ("simulationTime", "Simulation time in seconds", simulationTime);
	// cmd.Parse (argc, argv);


	cmd.Parse (argc, argv);

	//
	// Explicitly create the nodes required by the topology (shown above).
	//
	NS_LOG_INFO ("Create nodes.");
	NodeContainer c; // ALL Nodes
	c.Create(4);

	NodeContainer n2r1 = NodeContainer (c.Get (0), c.Get (1));
	NodeContainer r1r2 = NodeContainer (c.Get (1), c.Get (2));
	NodeContainer r2n3 = NodeContainer (c.Get (2), c.Get (3));

	//
	// Install Internet Stack
	//
	InternetStackHelper internet;
	internet.Install (c);
	// We create the channels first without any IP addressing information
	NS_LOG_INFO ("Create channels.");
	PointToPointHelper p2p;

	p2p.SetChannelAttribute ("Delay", StringValue (lat));
	p2p.SetDeviceAttribute ("DataRate", StringValue (rate));
	NetDeviceContainer d0d4 = p2p.Install (n2r1);
	NetDeviceContainer d4d5 = p2p.Install (r2n3);

	lat="50ms";
	rate="10Mbps";
	//drop tail queue
	p2p.SetQueue ("ns3::DropTailQueue");
	p2p.SetDeviceAttribute ("DataRate", StringValue (rate));
	p2p.SetChannelAttribute ("Delay", StringValue (lat));
	NetDeviceContainer d1d4 = p2p.Install (r1r2);

	NS_LOG_INFO ("Assign IP Addresses.");
	Ipv4AddressHelper ipv4;
	ipv4.SetBase ("10.1.1.0", "255.255.255.0");
	Ipv4InterfaceContainer i0i4 = ipv4.Assign (d0d4);

	ipv4.SetBase ("10.1.2.0", "255.255.255.0");
	Ipv4InterfaceContainer i1i4 = ipv4.Assign (d1d4);

	ipv4.SetBase ("10.1.3.0", "255.255.255.0");
	Ipv4InterfaceContainer i4i5 = ipv4.Assign (d4d5);

	NS_LOG_INFO ("Enable static global routing.");
	//
	// Turn on global static routing so we can actually be routed across the network.
	//
	Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


	NS_LOG_INFO ("Create Applications.");

	// TCP connection from N0 to N2
	uint16_t sinkPort[] = {11,12,13,14,15};
    int totalPorts=sizeof(sinkPort)/sizeof(sinkPort[0]);
	// uint16_t sinkPort = 8080;

    for (int i = 0; i < totalPorts; ++i)
    {
		Address sinkAddress (InetSocketAddress (i4i5.GetAddress(1), sinkPort[i])); // interface of n2
		PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), sinkPort[i]));
		ApplicationContainer sinkApps = packetSinkHelper.Install (c.Get(3)); //n3 as sink
		sinkApps.Start (Seconds (0.));
		sinkApps.Stop (Seconds (10.));

		Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (c.Get (0), TcpSocketFactory::GetTypeId ()); //source at n2

		// Trace Congestion window
		ns3TcpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeCallback (&CwndChange));

		// Create TCP application at n0
		Ptr<MyApp> app = CreateObject<MyApp> ();
		app->Setup (ns3TcpSocket, sinkAddress, packetSize, 100000, DataRate ("100Mbps"));
		c.Get (0)->AddApplication (app);
		app->SetStartTime (Seconds (1.));
		app->SetStopTime (Seconds (10.));
    }

	//
	// Now, do the actual simulation.
	//
	FlowMonitorHelper flowmon;
	Ptr<FlowMonitor> monitor = flowmon.InstallAll();
	NS_LOG_INFO ("Run Simulation.");
	Simulator::Stop (Seconds(10.0));
	Simulator::Run ();

	monitor->CheckForLostPackets ();
	Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
	std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
	int totalSumOfThroughput=0;
	int totalSquareSumThroughPut=0;
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
	//std::cout<<totalSquareSumThroughPut << " "<< totalSumOfThroughput << std::endl;
	std::string first= "+----------";
	std::string middle= "----------------";
	std::string last= "---------+";

	std::cout<<first+middle+last<<std::endl;

	float JainIndex = (totalSumOfThroughput*totalSumOfThroughput)/((float)totalPorts*totalSquareSumThroughPut);
	std::cout<<"Jain's Fairness Index: "<<JainIndex<<std::endl;
	monitor->SerializeToXmlFile("lab-1.flowmon", true, true);

	std::cout<<first<<middle+middle+middle<<last<<"\n" <<std::endl;

	// std::cout<<"Jain's Fairness Index: "<<JainIndex<<std::endl;
	monitor->SerializeToXmlFile("lab-1.flowmon", true, true);

	Simulator::Destroy ();
	NS_LOG_INFO ("Done.");

}