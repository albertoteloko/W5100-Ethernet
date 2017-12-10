#include "utility/w5100.h"
#include "Ethernet.h"
#include "Dhcp.h"

// XXX: don't make assumptions about the value of MAX_SOCK_NUM.
uint8_t EthernetClass::_state[MAX_SOCK_NUM] = { 
  0, 0, 0, 0 };
uint16_t EthernetClass::_server_port[MAX_SOCK_NUM] = { 
  0, 0, 0, 0 };

int EthernetClass::begin(uint8_t *mac_address, unsigned long timeout, unsigned long responseTimeout)
{
  static DhcpClass s_dhcp;
  _dhcp = &s_dhcp;


  // Initialise the basic info
  W5100.init();
  W5100.beginTransaction();
  W5100.setMACAddress(mac_address);
  W5100.setIPAddress(IPAddress(0,0,0,0));
  W5100.endTransaction();

  // Now try to get our config info from a DHCP server
  int ret = _dhcp->beginWithDHCP(mac_address, timeout, responseTimeout);
  if(ret == 1)
  {
    // We've successfully found a DHCP server and got our configuration info, so set things
    // accordingly
    W5100.beginTransaction();
    W5100.setIPAddress(_dhcp->getLocalIp());
    W5100.setGatewayIP(_dhcp->getGatewayIp());
    W5100.setSubnetMask(_dhcp->getSubnetMask());
    W5100.endTransaction();
    _dnsServerAddress = _dhcp->getDnsServerIp();
  }

  return ret;
}

void EthernetClass::begin(uint8_t *mac_address, IPAddress local_ip)
{
  // Assume the DNS server will be the machine on the same network as the local IP
  // but with last octet being '1'
  IPAddress dns_server = local_ip;
  dns_server[3] = 1;
  begin(mac_address, local_ip, dns_server);
}

void EthernetClass::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server)
{
  // Assume the gateway will be the machine on the same network as the local IP
  // but with last octet being '1'
  IPAddress gateway = local_ip;
  gateway[3] = 1;
  begin(mac_address, local_ip, dns_server, gateway);
}

void EthernetClass::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway)
{
  IPAddress subnet(255, 255, 255, 0);
  begin(mac_address, local_ip, dns_server, gateway, subnet);
}

void EthernetClass::begin(uint8_t *mac, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet)
{
  W5100.init();
  W5100.beginTransaction();
  W5100.setMACAddress(mac);
  W5100.setIPAddress(local_ip);
  W5100.setGatewayIP(gateway);
  W5100.setSubnetMask(subnet);
  W5100.endTransaction();
  _dnsServerAddress = dns_server;
}

int EthernetClass::maintain(){
  int rc = DHCP_CHECK_NONE;
  if(_dhcp != NULL){
    //we have a pointer to dhcp, use it
    rc = _dhcp->checkLease();
    switch ( rc ){
      case DHCP_CHECK_NONE:
        //nothing done
        break;
      case DHCP_CHECK_RENEW_OK:
      case DHCP_CHECK_REBIND_OK:
        //we might have got a new IP.
        W5100.beginTransaction();
        W5100.setIPAddress(_dhcp->getLocalIp());
        W5100.setGatewayIP(_dhcp->getGatewayIp());
        W5100.setSubnetMask(_dhcp->getSubnetMask());
        W5100.endTransaction();
        _dnsServerAddress = _dhcp->getDnsServerIp();
        break;
      default:
        //this is actually a error, it will retry though
        break;
    }
  }
  return rc;
}

IPAddress EthernetClass::localIP()
{
  W5100.beginTransaction();
  IPAddress ret = W5100.getIPAddress();
  W5100.endTransaction();
  return ret;
}

IPAddress EthernetClass::subnetMask()
{
  W5100.beginTransaction();
  IPAddress ret = W5100.getSubnetMask();
  W5100.endTransaction();
  return ret;
}

IPAddress EthernetClass::gatewayIP()
{
  W5100.beginTransaction();
  IPAddress ret = W5100.getGatewayIP();
  W5100.endTransaction();
  return ret;
}

IPAddress EthernetClass::dnsServerIP()
{
  return _dnsServerAddress;
}

EthernetClass Ethernet;
