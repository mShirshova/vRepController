#ifndef AGENTUTIL_H
#define	AGENTUTIL_H

#include <Windows.h>
#include <string>
#include "Parameters.h"
#include "Util.h"
#include "LOG.h"
#include "Generation.h"
#include <time.h>
#include "Statistics.h"
#include "AzGraphCtrl.h"

using namespace std;

namespace AgentUtil
{
	extern int processes_num;
	extern int robots_num_in_process;
	extern int cur_session;
	extern int cur_lunch;
	extern int _mode;
	extern int cur_show_agent;
	extern int cur_graph_type;
	extern int cur_graph_selection;
	extern int cur_program_version;
	extern GUID guid;
	extern bool agentsAreLoaded;
	extern bool clearStatBuffer;
	extern bool statsIsLoaded;
	
	extern Statistic curStatistic;
	extern vector<Statistic> loadedStatistcis;

	extern STARTUPINFO si_show;
	extern PROCESS_INFORMATION pi_show;
	extern vector<STARTUPINFO> si;
	extern vector<PROCESS_INFORMATION> pi;

	//events
	extern vector<HANDLE> hEvtsReadNetworkAndReset;
	extern vector<HANDLE> hEvtsResetAndStopAgent;
	extern vector<HANDLE> hEvtsAgentIsReady;
	extern vector<HANDLE> hEvtsStartLife;
	extern vector<HANDLE> hEvtsAgentLifeEnded;
	extern vector<HANDLE> hEvtsExitProcess;
	extern vector<HANDLE> hEvtsAgetnProcessCleared;

	//file mapping
	extern vector<HANDLE> hfileMappings;
	extern vector<PVOID> pArray;

    extern HANDLE hfileMappings_show;
	extern PVOID pArray_show;

	void initialize();
	void resetLaunch();
	void resetBeforeNewGeneration();
	void clear();
	void clearAfterShowAgent();
	void createAgentProcesses(HWND hwnd);
	void createShowAgentProcess(HWND hwnd);
	void terminateAgentProcesses();
	void terminateShowAgentProcess();
	void initializeEvents();
	bool checkAgentIsActive(int agentId);
	bool hasNextSession();
	void setNextSession();
	bool writeNetworksInFiles(Generation& g); 
	void saveAgentsToFile(Generation& g, string file_name);
	bool loadAgentsFromFile(Generation& g, string file_name);
	void loadAgent(HANDLE& hFile, AgentInfo& agent);
	void saveBestAgentToFile(Generation& g, string file_name);
	bool loadBestAgentFromFile(Generation& g, string file_name);
	bool writeDescendantInFile2(HANDLE& hFile, AgentInfo& agent);
	bool writeDescendantInFile(char*& ptr, int& size, AgentInfo& agent);

	void showBestAgent(Generation& g, HWND hwnd);
	void showNextAgent(Generation& g, HWND hwnd);
	void showPrevAgent(Generation& g, HWND hwnd);
	
	void saveDefaultParams();
	bool loadDefaultParams();
	void saveParamsToFile(HANDLE& hFile);
	void loadParamsFromFile(HANDLE& hFile);
	void saveParamsToFile(string file_name);
	void loadParamsFromFile(string file_name);

	void plotGraph(AzGraphCtrl& m_graph, int graphPointType, int descNum = 0);
	void plotStatisticGraph(vector<Statistic>&st, AzGraphCtrl& m_graph, int graphPointType, int selection, vector<string>* lgnds = NULL);
	void renameLegend(int legNum, string name, AzGraphCtrl& m_graph, int graphPointType);
	void plotJointDataGraphs(AzGraphCtrl& m_graph, int graphPointType, int descNum, int jointNum);
	void plotOutNeuronDataGraphs(AzGraphCtrl& m_graph, int graphPointType, int descNum, int jointNum);
	void saveResults(string file_name, int programVersion);
	void calcAvarageStat(Statistic& stat);
	void saveAvStatistic(string file_name);
	void loadResults(string file_name);

	void saveFromStatistic(int curSelection);
	void saveFromNetwork(Generation& g, int curSelection, bool forBestAgent = false);

	void readAgentLifeInfo(Generation& g);
	void sendMessagesToAgents(int command);
	void waitForMessagesFromAgents(int command);
	BOOL CALLBACK closeEnumProc(HWND hwnd, LPARAM lParam);

	char* sendMessageToShowAgent(int mess, int& size);

	void writeResultsInFile(Generation& g);
	void writeTimeOfCalculation(time_t startTime, time_t endTime);

	void saveRuntimeInfo(Generation& g);

	void setMode(int m);
	int getMode();

	bool writeString(char*& from, string& inf, int& size);
	bool writeInt(char*& from,int& inf,int& size);
	bool writeDouble(char*& from,double& inf, int& size);
	bool writeBool(char*& from,bool& inf, int& size);
	void readInt(char*& from, int& inf);
	void readDouble(char*& from,double& inf);
	void readBool(char*& from,bool& inf);
};

#endif

