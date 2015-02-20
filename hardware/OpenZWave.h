#pragma once

#ifdef WITH_OPENZWAVE

#include <map>
#include <string>
#include <time.h>
#include "ZWaveBase.h"
#include "ASyncSerial.h"
#include <list>
#include "openzwave/control_panel/ozwcp.h"

namespace OpenZWave
{
	class ValueID;
	class Manager;
	class Notification;
}

namespace Json
{
	class Value;
}

class COpenZWave : public AsyncSerial, public ZWaveBase, public COpenZWaveControlPanel
{
public:
	typedef struct  
	{
		std::list<OpenZWave::ValueID>	Values;
		time_t							m_LastSeen;
	}NodeCommandClass;
	
	typedef enum
	{
		NTSATE_UNKNOWN=0,
		NSTATE_AWAKE,
		NSTATE_SLEEP,
		NSTATE_DEAD
	} eNodeState;

	typedef struct
	{
		unsigned int					m_homeId;
		unsigned char					m_nodeId;
		bool							m_polled;

		std::string						szType;
		int								iVersion;
		std::string						Manufacturer_id;
		std::string						Manufacturer_name;
		std::string						Product_type;
		std::string						Product_id;
		std::string						Product_name;

		std::map<int, std::map<int, NodeCommandClass> >	Instances;

		eNodeState						eState;

		bool							HaveUserCodes;

		time_t							m_LastSeen;

		//Thermostat settings
		int								tClockDay;
		int								tClockHour;
		int								tClockMinute;
		int								tMode;
		int								tFanMode;
		std::vector<string>				tModes;
		std::vector<string>				tFanModes;
	}NodeInfo;

	COpenZWave(const int ID, const std::string& devname);
	~COpenZWave(void);

	bool GetInitialDevices();
	bool GetUpdates();
	void OnZWaveNotification( OpenZWave::Notification const* _notification);
	void OnZWaveDeviceStatusUpdate(int cs, int err);
	void EnableDisableNodePolling();
	void SetNodeName(const unsigned int homeID, const int nodeID, const std::string &Name);
	std::string GetNodeStateString(const unsigned int homeID, const int nodeID);
	void GetNodeValuesJson(const unsigned int homeID, const int nodeID, Json::Value &root, const int index);
	NodeInfo* GetNodeInfo( const unsigned int homeID, const int nodeID );
	bool ApplyNodeConfig(const unsigned int homeID, const int nodeID, const std::string &svaluelist);

	std::string GetVersion();

	bool SetUserCodeEnrollmentMode();
	bool GetNodeUserCodes(const unsigned int homeID, const int nodeID, Json::Value &root);
	bool RemoveUserCode(const unsigned int homeID, const int nodeID, const int codeIndex);

	std::string GetSupportedThermostatModes(const unsigned long ID);
	std::string GetSupportedThermostatFanModes(const unsigned long ID);

	//Controller Commands
	bool RequestNodeConfig(const unsigned int homeID, const int nodeID);
	bool RemoveFailedDevice(const int nodeID);
	bool ReceiveConfigurationFromOtherController();
	bool SendConfigurationToSecondaryController();
	bool TransferPrimaryRole();
	bool CancelControllerCommand();
	bool IncludeDevice();
	bool ExcludeDevice(const int nodeID);
	bool SoftResetDevice();
	bool HardResetDevice();
	bool HealNetwork();
	bool HealNode(const int nodeID);
	bool GetFailedState();
	bool NetworkInfo(const int hwID,std::vector< std::vector< int > > &NodeArray);
	int ListGroupsForNode(const int nodeID);
	int ListAssociatedNodesinGroup(const int nodeID,const int groupID,std::vector< int > &nodesingroup);
	bool AddNodeToGroup(const int nodeID,const int groupID, const int addID);
	bool RemoveNodeFromGroup(const int nodeID,const int groupID, const int removeID);
	std::string GetConfigFile(std::string &szConfigFile);

	void NightlyNodeHeal();

	bool m_awakeNodesQueried;
	bool m_allNodesQueried;

	unsigned char m_controllerNodeId;
private:
	void NodesQueried();
	void DeleteNode(const unsigned int homeID, const int nodeID);
	void AddNode(const unsigned int homeID, const int nodeID,const NodeInfo *pNode);
	void EnableNodePoll(const unsigned int homeID, const int nodeID, const int pollTime);
	void DisableNodePoll(const unsigned int homeID, const int nodeID);
	bool GetValueByCommandClass(const int nodeID, const int instanceID, const int commandClass, OpenZWave::ValueID &nValue);
	bool GetValueByCommandClassLabel(const int nodeID, const int instanceID, const int commandClass, const std::string &vLabel, OpenZWave::ValueID &nValue);
	bool GetNodeConfigValueByIndex(const NodeInfo *pNode, const int index, OpenZWave::ValueID &nValue);
	void AddValue(const OpenZWave::ValueID &vID);
	void UpdateValue(const OpenZWave::ValueID &vID);
	void UpdateNodeEvent(const OpenZWave::ValueID &vID, int EventID);
	void UpdateNodeScene(const OpenZWave::ValueID &vID, int SceneID);
	NodeInfo* GetNodeInfo( OpenZWave::Notification const* _notification );
	void SwitchLight(const int nodeID, const int instanceID, const int commandClass, const int value);
	void SwitchColor(const int nodeID, const int instanceID, const int commandClass, const unsigned char *colvalues, const unsigned char valuelen);
	void SetThermostatSetPoint(const int nodeID, const int instanceID, const int commandClass, const float value);
	void SetClock(const int nodeID, const int instanceID, const int commandClass, const int day, const int hour, const int minute);
	void SetThermostatMode(const int nodeID, const int instanceID, const int commandClass, const int tMode);
	void SetThermostatFanMode(const int nodeID, const int instanceID, const int commandClass, const int fMode);

	unsigned char GetInstanceFromValueID(const OpenZWave::ValueID &vID);

	bool IsNodeRGBW(const unsigned int homeID, const int nodeID);

	void StopHardwareIntern();

	void EnableDisableDebug();

	bool OpenSerialConnector();
	void CloseSerialConnector();

	void WriteControllerConfig();
	time_t m_LastControllerConfigWrite;

	OpenZWave::Manager *m_pManager;

	std::list<NodeInfo> m_nodes;

	std::string m_szSerialPort;
	unsigned int m_controllerID;

	bool m_bIsShuttingDown;
	bool m_initFailed;
	bool m_bInUserCodeEnrollmentMode;
	bool m_bNightlyNetworkHeal;
	bool m_bNeedSave;
};

#endif //WITH_OPENZWAVE

