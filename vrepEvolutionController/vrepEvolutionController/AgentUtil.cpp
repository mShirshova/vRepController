#include "AgentUtil.h"
#include "Strsafe.h"
#include <sstream>

int AgentUtil::processes_num;
int AgentUtil::robots_num_in_process;
int AgentUtil::cur_session;
int AgentUtil::cur_lunch;
int AgentUtil::cur_show_agent;
int AgentUtil::cur_graph_type = 0;
int AgentUtil::cur_graph_selection = 0;
int AgentUtil::cur_program_version;
int AgentUtil::_mode = Constants::IDLE_MODE;
GUID AgentUtil::guid;
bool AgentUtil::agentsAreLoaded = false;
bool AgentUtil::clearStatBuffer = true;
bool AgentUtil::statsIsLoaded = false;

Statistic AgentUtil::curStatistic;
vector<Statistic> AgentUtil::loadedStatistcis;

vector<STARTUPINFO> AgentUtil::si;
vector<PROCESS_INFORMATION> AgentUtil::pi;

STARTUPINFO AgentUtil::si_show;
PROCESS_INFORMATION AgentUtil::pi_show;

//events
vector<HANDLE> AgentUtil::hEvtsReadNetworkAndReset;
vector<HANDLE> AgentUtil::hEvtsResetAndStopAgent;
vector<HANDLE> AgentUtil::hEvtsAgentIsReady;
vector<HANDLE> AgentUtil::hEvtsStartLife;
vector<HANDLE> AgentUtil::hEvtsAgentLifeEnded;
vector<HANDLE> AgentUtil::hEvtsExitProcess;
vector<HANDLE> AgentUtil::hEvtsAgetnProcessCleared;

//file mapping
vector<HANDLE> AgentUtil::hfileMappings;
vector<PVOID> AgentUtil::pArray;

HANDLE AgentUtil::hfileMappings_show;
PVOID AgentUtil::pArray_show;

//called befeore every launch
void AgentUtil::resetLaunch()
{
	cur_session = -1;

	robots_num_in_process = ceil((double)Parameters::DESCENDANT_NUM/Parameters::PROCCESSES_NUM);

	string s = Util::to_string(Parameters::GENERATION_NUM)+"_D"+Util::to_string(Parameters::DESCENDANT_NUM);
}

void AgentUtil::resetBeforeNewGeneration()
{
	cur_session = -1;
}


//called once
void AgentUtil::initialize()
{
	cur_lunch = 0;
	processes_num = Parameters::PROCCESSES_NUM;//min(Parameters::PROCCESSES_NUM, Parameters::DESCENDANT_NUM);

	initializeEvents();
	hfileMappings.resize(processes_num);
	pArray.resize(processes_num);
	string name;
	for(int i=0; i<processes_num; i++)
	{
		name = Constants::BASE_FILE_NAME + '_' + Util::to_string(i) +  '_' + Util::guidToString(guid);
		hfileMappings[i] = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,Constants::SHARED_MEMORY_SIZE,name.data());
		pArray[i] = MapViewOfFile(hfileMappings[i], FILE_MAP_ALL_ACCESS,0,0,Constants::SHARED_MEMORY_SIZE);
	}
}

void AgentUtil::clear()
{
	for(int i=0; i<processes_num; i++)
	{
		UnmapViewOfFile(pArray[i]);

		CloseHandle(hEvtsReadNetworkAndReset[i]);
		CloseHandle(hEvtsResetAndStopAgent[i]);
		CloseHandle(hEvtsAgentIsReady[i]);
		CloseHandle(hEvtsStartLife[i]);
		CloseHandle(hEvtsAgentLifeEnded[i]);
		CloseHandle(hEvtsExitProcess[i]);
		CloseHandle(hEvtsAgetnProcessCleared[i]);

		CloseHandle(hfileMappings[i]);
		CloseHandle(pi[i].hProcess);
		CloseHandle(pi[i].hThread);
	}
}

void AgentUtil::clearAfterShowAgent()
{
	UnmapViewOfFile(pArray_show);

	CloseHandle(hfileMappings_show);
	CloseHandle(pi_show.hProcess);
	CloseHandle(pi_show.hThread);
}


void AgentUtil::initializeEvents()
{
	hEvtsReadNetworkAndReset.resize(processes_num);
	hEvtsResetAndStopAgent.resize(processes_num);
	hEvtsAgentIsReady.resize(processes_num);
	hEvtsStartLife.resize(processes_num);
	hEvtsAgentLifeEnded.resize(processes_num);
	hEvtsExitProcess.resize(processes_num);
	hEvtsAgetnProcessCleared.resize(processes_num);

	string id = Util::guidToString(guid);
	string name;
	for(int i=0; i<processes_num; i++)
	{
		name = Constants::READ_NETWORK_AND_RESET + '_' + Util::to_string(i) + '_' + id;
		hEvtsReadNetworkAndReset[i] = CreateEvent(NULL,FALSE,FALSE,name.data());
		name = Constants::RESET_AND_STOP + '_' + Util::to_string(i) + '_' + id;
		hEvtsResetAndStopAgent[i] = CreateEvent(NULL,FALSE,FALSE, name.data());
		name = Constants::AGENT_IS_READY + '_' + Util::to_string(i) + '_' + id;
		hEvtsAgentIsReady[i] = CreateEvent(NULL,FALSE,FALSE,name.data());
		name = Constants::START_LIFE + '_' + Util::to_string(i) + '_' + id;
		hEvtsStartLife[i] =  CreateEvent(NULL,FALSE,FALSE,name.data());
		name = Constants::AGENT_LIFE_ENDED + '_' + Util::to_string(i) + '_' + id;
		hEvtsAgentLifeEnded[i] = CreateEvent(NULL,FALSE,FALSE,name.data());
		name = Constants::EXIT_PROCESS + '_' + Util::to_string(i) + '_' + id;
		hEvtsExitProcess[i] = CreateEvent(NULL,FALSE,FALSE, name.data());
		name = Constants::AGENT_PROCESS_CLEARED + '_' + Util::to_string(i) + '_' + id;
		hEvtsAgetnProcessCleared[i] = CreateEvent(NULL,FALSE,FALSE, name.data());
	}
}

void AgentUtil::createAgentProcesses(HWND hwnd)
{
	const int buffsize = 100;
	TCHAR chNewEnv[buffsize];
	LPTSTR lpszCurrentVariable; 

	int robotId = 0;
	string file_name;
	string commandLine;
	string parentId = "parentId="+Util::guidToString(guid);
	string childId;
	string naoNum = "naoNum="+Util::to_string(Parameters::NAO_NUM_IN_SCENE);
	string modeS = "mode="+Util::to_string(_mode);

	si.resize(processes_num);
	pi.resize(processes_num);

	for(int i = 0; i < processes_num; i++)
	{
		file_name = "asti.ttt";//"E:\\programming\\vrep\\nao\\asti.ttt";
		//file_name = "C:\\Users\\Shirshova\\Documents\\program\\vrep\\nao\\asti.ttt";

		commandLine = "C:\\Program Files (x86)\\V-REP3\\V-REP_PRO_EDU\\vrep.exe " +file_name;

		lpszCurrentVariable = (LPTSTR) chNewEnv; 
		StringCchCopy(lpszCurrentVariable, buffsize, TEXT(parentId.data()));
		lpszCurrentVariable = lpszCurrentVariable + parentId.size() + 1;
		childId = "childId=" + Util::to_string(robotId);
		StringCchCopy(lpszCurrentVariable, buffsize, TEXT(childId.data()));
		lpszCurrentVariable = lpszCurrentVariable + childId.size() + 1;
		StringCchCopy(lpszCurrentVariable, buffsize, TEXT(naoNum.data()));
		lpszCurrentVariable = lpszCurrentVariable + naoNum.size() + 1;
		StringCchCopy(lpszCurrentVariable, buffsize, TEXT(modeS.data()));
		lpszCurrentVariable = lpszCurrentVariable + modeS.size() + 1;
		*lpszCurrentVariable = (TCHAR)0;

		ZeroMemory(&si[i], sizeof(si[i]));
		si[i].cb = sizeof(si);
		bool sucsess = CreateProcess(NULL, (LPSTR)commandLine.data(), NULL, NULL, FALSE,
			0,(LPVOID) chNewEnv,NULL,&si[i],&pi[i]);

		DWORD_PTR mask = 1 << i;//1 << 2*i;
		bool setMask = SetProcessAffinityMask(pi[i].hProcess, mask);

		DWORD lastError = GetLastError();

		if(!sucsess)
			MessageBox(hwnd,"Error of Creating process",NULL,MB_OK);

		robotId++;
	}
}

void AgentUtil::createShowAgentProcess(HWND hwnd)
{
	DWORD exidCode;
	GetExitCodeProcess(pi_show.hProcess, &exidCode);

	if(exidCode != STILL_ACTIVE)
	{
		const int buffsize = 100;
		TCHAR chNewEnv[buffsize];
		LPTSTR lpszCurrentVariable; 

		string file_name;
		string commandLine;
		string parentId = "parentId="+Util::guidToString(guid);
		string childId;
		string naoNum = "naoNum="+Util::to_string(1);
		string modeS = "mode="+Util::to_string(_mode);
	
		file_name = "astiShow.ttt";//"astiShow.ttt";//"E:\\programming\\vrep\\nao\\astiShow.ttt";
		//file_name = "C:\\Users\\Shirshova\\Documents\\program\\vrep\\nao\\astiShow.ttt";

		commandLine = "C:\\Program Files (x86)\\V-REP3\\V-REP_PRO_EDU\\vrep.exe " +file_name;

		lpszCurrentVariable = (LPTSTR) chNewEnv; 
		StringCchCopy(lpszCurrentVariable, buffsize, TEXT(parentId.data()));
		lpszCurrentVariable = lpszCurrentVariable + parentId.size() + 1;
		childId = "childId=show";
		StringCchCopy(lpszCurrentVariable, buffsize, TEXT(childId.data()));
		lpszCurrentVariable = lpszCurrentVariable + childId.size() + 1;
		StringCchCopy(lpszCurrentVariable, buffsize, TEXT(naoNum.data()));
		lpszCurrentVariable = lpszCurrentVariable + naoNum.size() + 1;
		StringCchCopy(lpszCurrentVariable, buffsize, TEXT(modeS.data()));
		lpszCurrentVariable = lpszCurrentVariable + modeS.size() + 1;
		*lpszCurrentVariable = (TCHAR)0;

		ZeroMemory(&si_show, sizeof(si_show));
		si_show.cb = sizeof(si);
		bool sucsess = CreateProcess(NULL, (LPSTR)commandLine.data(), NULL, NULL, FALSE,
			0,(LPVOID) chNewEnv,NULL,&si_show,&pi_show);

		if(!sucsess)
			MessageBox(hwnd,"Error of Creating process",NULL,MB_OK);	

		string name;
	
		name = Constants::BASE_FILE_NAME + "_show_" + Util::guidToString(guid);
		hfileMappings_show = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,Constants::SHARED_MEMORY_SIZE,name.data());
		pArray_show = MapViewOfFile(hfileMappings_show, FILE_MAP_ALL_ACCESS,0,0,Constants::SHARED_MEMORY_SIZE);
	
		cur_show_agent = 0;
	}
}

void AgentUtil::terminateAgentProcesses()
{
	DWORD exitCode;
	for(int i = 0; i < processes_num; i++)
	{
		GetExitCodeProcess(pi[i].hProcess, &exitCode);
		if(exitCode == STILL_ACTIVE)
			TerminateProcess(pi[i].hProcess,0);
	}
}

void AgentUtil::terminateShowAgentProcess()
{
	DWORD exitCode;

	GetExitCodeProcess(pi_show.hProcess, &exitCode);
	if(exitCode == STILL_ACTIVE)
		TerminateProcess(pi_show.hProcess,0);
}

bool AgentUtil::writeNetworksInFiles(Generation& g)
{
	char* p;
	for(int i=0; i<processes_num; i++)
	{
		bool sucsess = false;
		p = (char*)pArray[i];
		if(p != 0)
		{
			sucsess = true;
			int desc_num = (robots_num_in_process-1)*processes_num + i;
			int num = (desc_num < Parameters::DESCENDANT_NUM)? robots_num_in_process : (robots_num_in_process - 1);
			int size = 0;
			writeInt(p, num, size);

			for(int j=0; j<robots_num_in_process; j++)
			{
				desc_num = j*processes_num + i;
				if(desc_num < Parameters::DESCENDANT_NUM)
				{
					AgentInfo& desc = g.descendances[desc_num];
					sucsess = writeDescendantInFile(p, size, desc);
					if(!sucsess)
						break;
				}
			}
		}
		if(!sucsess)
			return false;
	}
	return true;
}

/*
	The structure of writed agent:
	nao num in scene;
	agent life time,
	type of control,
	type of network,
	type of neuron,
	dimention num,
	inputTransmFuncType,
	num of neurons,
	neuron1: numOfConnection,
		     id, 
			 type,
			 neuronParams, 
			 connection1: typeOfConnection,
						 typeOfNeuron,
						 neuronId,
						 weight
			 connection2: -//-//-
	neuron2: -//-//-
*/
bool AgentUtil::writeDescendantInFile(char*& ptr, int& size, AgentInfo& agent)
{
	bool success = true;
	success = writeInt(ptr, Parameters::ONE_AGENT_LIFE_TIME, size);
	success = writeDouble(ptr, Parameters::OUT_MILTIPLY_COEFF, size);
	success = writeDouble(ptr, Parameters::ANGLE_STEP_SIZE, size);
	success = writeInt(ptr, Parameters::typeOfControl, size);
	success = writeInt(ptr, Parameters::networkType, size);
	success = writeInt(ptr, Parameters::neuronType, size);
	success = writeInt(ptr, Parameters::dimentionOfInput, size);
	success = writeInt(ptr, Parameters::inputTransmissionFuncType, size);
	success = writeBool(ptr, Parameters::showJointsData, size);
	int num =  agent.neurons.size();
	success = writeInt(ptr, num, size);
    
	int con_num=0;
	for(int i=0; i<num; i++)
	{
		con_num = agent.neurons[i].connections.size();
		success = writeInt(ptr, con_num, size);
		success = writeInt(ptr, agent.neurons[i].id, size);
		success = writeInt(ptr, agent.neurons[i].type, size);
	
		//neuron params
		NeuronParams& np = agent.neurons[i].neuronParams;

		success = writeBool(ptr, np.useARConnections, size);
		success = writeBool(ptr, np.useNormalization, size);
		success = writeBool(ptr, np.useRandomAddition, size);

		success = writeBool(ptr, np.isIncreasing, size);

		success = writeInt(ptr, np.jNum, size);

		success = writeInt(ptr, np.transmissionFuncType, size);

		success = writeDouble(ptr, np.delayTime, size);
		success = writeDouble(ptr, np.threshold, size);
		success = writeDouble(ptr, np.resThreshold, size);
		success = writeDouble(ptr, np.logCoeff, size);
		success = writeDouble(ptr, np.uRand, size);
		success = writeDouble(ptr, np.aRand, size);

		success = writeDouble(ptr, np.minPotential, size);
		success = writeDouble(ptr, np.maxPotential, size);
		success = writeDouble(ptr, np.frequency, size);

		success = writeDouble(ptr, np.lBound, size);
		success = writeDouble(ptr, np.rBound, size);
		success = writeDouble(ptr, np.boundVal, size);

		success = writeInt(ptr, np.delayNum, size);
		
		//connections
		for(int j=0; j<con_num; j++)
		{
			Connection& c = agent.neurons[i].connections[j]; 

			success = writeInt(ptr, c.typeOfConnection, size);
			success = writeInt(ptr, c.typeOfNeuron, size);
			success = writeInt(ptr, c.neuronId, size);
			success = writeDouble(ptr, c.weight, size);
		}

		//check sucess only after attemt to write a whole neuron (to avid a frecuent writening)
		if(!success)
			return false;
	}
	return true;
}

void AgentUtil::readAgentLifeInfo(Generation& g)
{
	char* ptr;

	if(Parameters::showJointsData)
	{
		if(curStatistic.jointValues.size() != Parameters::DESCENDANT_NUM)
		{
			curStatistic.jointValues.resize(Parameters::DESCENDANT_NUM);
			curStatistic.outNetwValuesValues.resize(Parameters::DESCENDANT_NUM);
		}

		for(int i=0; i<Parameters::DESCENDANT_NUM; ++i)
		{
			curStatistic.jointValues[i].resize(Parameters::JOINT_NUM);
			curStatistic.outNetwValuesValues[i].resize(Parameters::OUTPUT_NUM);
		}
	}

	for(int i=0; i<processes_num; i++)
	{
		ptr = (char*)pArray[i];
		if(ptr != 0)
		{
			int desc_num;
			double temp=0;
			for(int j=0; j<robots_num_in_process; j++)
			{
				desc_num = j*processes_num + i;
				if(desc_num < Parameters::DESCENDANT_NUM)
				{
					readDouble(ptr, temp);
					g.descendances[desc_num].activity = temp;
					readDouble(ptr, temp);
					g.descendances[desc_num].distance = temp;
					g.calcFitness(g.descendances[desc_num]);
					readDouble(ptr, temp);
					g.descendances[desc_num].netwConsistency = temp;
					readDouble(ptr, temp);
					g.descendances[desc_num].timeOfAchievingResults = temp;
					readDouble(ptr, temp);
					g.descendances[desc_num].avHeight = temp;
				}
			}

			if(Parameters::showJointsData)
			{
				//read joint values
				for(int j=0; j<robots_num_in_process; j++)
				{
					desc_num = j*processes_num + i;
					if(desc_num < Parameters::DESCENDANT_NUM)
					{
						for(int k=0; k<Parameters::JOINT_NUM; ++k)
						{
							int size = 0;
							double value;
							readInt(ptr, size);
							curStatistic.jointValues[desc_num][k].resize(size);
							for(int n=0; n<size; ++n)
							{
								readDouble(ptr, value);
								curStatistic.jointValues[desc_num][k][n] = value;
							}
						}
					}
				}

				//read network output values
				for(int j=0; j<robots_num_in_process; j++)
				{
					desc_num = j*processes_num + i;
					if(desc_num < Parameters::DESCENDANT_NUM)
					{
						for(int k=0; k<Parameters::OUTPUT_NUM; ++k)
						{
							int size = 0;
							double value;
							readInt(ptr, size);
							curStatistic.outNetwValuesValues[desc_num][k].resize(size);
							for(int n=0; n<size; ++n)
							{
								readDouble(ptr, value);
								curStatistic.outNetwValuesValues[desc_num][k][n] = value;
							}
						}
					}
				}
			}
		}
		else
		{
			LOG::startFileInput();
			LOG::writeMessageInFile("AgentUtil::readInfoFromFiles: ERROR!!!!");
			LOG::endFileInput();
		}
	}
}

void AgentUtil::saveAgentsToFile(Generation& g, string file_name)
{
	HANDLE hFile; 
	DWORD wmWritten; 
	hFile = CreateFile(file_name.data(), GENERIC_READ|GENERIC_WRITE, 
	FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL); 

	saveParamsToFile(hFile);

	for(int i=0; i<g.descensantNum; i++)
		writeDescendantInFile2(hFile, g.descendances[i]);

	CloseHandle(hFile); 
}

void AgentUtil::saveBestAgentToFile(Generation& g, string file_name)
{
	if(g.descendances.size() > 0)
	{
		HANDLE hFile; 
		hFile = CreateFile(file_name.data(), GENERIC_READ|GENERIC_WRITE, 
		FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL); 

		saveParamsToFile(hFile);

		writeDescendantInFile2(hFile, curStatistic.findBestAgent(g));

		CloseHandle(hFile); 
	}
}

bool AgentUtil::writeDescendantInFile2(HANDLE& hFile, AgentInfo& agent)
{
	DWORD wmWritten; 
	
	WriteFile(hFile, (char*)&agent.fitness, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&agent.distance, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&agent.activity, (DWORD)sizeof(double), &wmWritten,NULL);

	WriteFile(hFile, (char*)&agent.netwConsistency, (DWORD)sizeof(double), &wmWritten,NULL);

	int num =  agent.neurons.size();
	WriteFile(hFile, (char*)&num, (DWORD)sizeof(int), &wmWritten,NULL);
    
	int con_num=0;
	for(int i=0; i<num; i++)
	{
		con_num = agent.neurons[i].connections.size();
		WriteFile(hFile,(char*)&con_num, (DWORD)sizeof(int), &wmWritten,NULL);

		WriteFile(hFile,(char*)&agent.neurons[i].id, (DWORD)sizeof(int), &wmWritten,NULL);
		WriteFile(hFile,(char*)&agent.neurons[i].type, (DWORD)sizeof(int), &wmWritten,NULL);
	
		//neuron params
		NeuronParams& np = agent.neurons[i].neuronParams;

		WriteFile(hFile,(char*)&np.useARConnections,(DWORD)sizeof(bool), &wmWritten,NULL);
		WriteFile(hFile,(char*)&np.useNormalization, (DWORD)sizeof(bool), &wmWritten,NULL);
		WriteFile(hFile,(char*)&np.useRandomAddition, (DWORD)sizeof(bool), &wmWritten,NULL);

		WriteFile(hFile,(char*)&np.isIncreasing, (DWORD)sizeof(bool), &wmWritten,NULL);

		WriteFile(hFile,(char*)&np.jNum, (DWORD)sizeof(int), &wmWritten,NULL);

		WriteFile(hFile,(char*)&np.transmissionFuncType, (DWORD)sizeof(int), &wmWritten,NULL);

		WriteFile(hFile,(char*)&np.delayTime,(DWORD)sizeof(double), &wmWritten,NULL);
		WriteFile(hFile,(char*)&np.threshold, (DWORD)sizeof(double), &wmWritten,NULL);
		WriteFile(hFile,(char*)&np.resThreshold, (DWORD)sizeof(double), &wmWritten,NULL);
		WriteFile(hFile,(char*)&np.logCoeff, (DWORD)sizeof(double), &wmWritten,NULL);
		WriteFile(hFile,(char*)&np.uRand,(DWORD)sizeof(double), &wmWritten,NULL);
		WriteFile(hFile,(char*)&np.aRand,(DWORD)sizeof(double), &wmWritten,NULL);

		WriteFile(hFile,(char*)&np.minPotential,(DWORD)sizeof(double), &wmWritten,NULL);
		WriteFile(hFile,(char*)&np.maxPotential,(DWORD)sizeof(double), &wmWritten,NULL);
		WriteFile(hFile,(char*)&np.frequency,(DWORD)sizeof(double), &wmWritten,NULL);

		WriteFile(hFile,(char*)&np.lBound,(DWORD)sizeof(double), &wmWritten,NULL);
		WriteFile(hFile,(char*)&np.rBound,(DWORD)sizeof(double), &wmWritten,NULL);
		WriteFile(hFile,(char*)&np.boundVal,(DWORD)sizeof(double), &wmWritten,NULL);

		WriteFile(hFile,(char*)&np.delayNum,(DWORD)sizeof(int), &wmWritten,NULL);

		//connections
		for(int j=0; j<con_num; j++)
		{
			Connection& c = agent.neurons[i].connections[j]; 

			WriteFile(hFile,(char*)&c.typeOfConnection, (DWORD)sizeof(int), &wmWritten,NULL);
			WriteFile(hFile,(char*)&c.typeOfNeuron, (DWORD)sizeof(int), &wmWritten,NULL);
			WriteFile(hFile,(char*)&c.neuronId,  (DWORD)sizeof(int), &wmWritten,NULL);
			WriteFile(hFile,(char*)&c.weight,(DWORD)sizeof(double), &wmWritten,NULL);
		}
	}
	return true;
}

bool AgentUtil::loadBestAgentFromFile(Generation& g, string file_name)
{
	HANDLE hFile; 
	DWORD wmWritten; 
	hFile = CreateFileA(file_name.data(),GENERIC_READ|GENERIC_WRITE, 
	FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	loadParamsFromFile(hFile);

	g.clearDescendants();

	g.descensantNum = Parameters::DESCENDANT_NUM;
	g.generationNum = Parameters::GENERATION_NUM;
	g.curDescendant = 0;
	g.curGeneration = 0;

	AgentInfo agent;
	agent.neurons.networkParams.neuronType = Parameters::neuronType;
	loadAgent(hFile, agent);
	g.createGenerationFromOneDescendant(agent);

	return true;
}

bool AgentUtil::loadAgentsFromFile(Generation& g, string file_name)
{
	HANDLE hFile; 
	DWORD wmWritten; 
	hFile = CreateFileA(file_name.data(),GENERIC_READ|GENERIC_WRITE, 
	FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL); 
	
	loadParamsFromFile(hFile);

	g.descensantNum = Parameters::DESCENDANT_NUM;
	g.generationNum = Parameters::GENERATION_NUM;
	g.descendances.resize(g.descensantNum);
	g.curDescendant = 0;
	g.curGeneration = 0;
	

	for(int k=0; k<g.descensantNum; ++k)
	{
		g.descendances[k].neurons.networkParams.neuronType = Parameters::neuronType;
		loadAgent(hFile, g.descendances[k]);
	}

	CloseHandle(hFile); 
	curStatistic.updateSortAgents(g);
	return true;
}

void AgentUtil::loadAgent(HANDLE& hFile, AgentInfo& agent)
{
	DWORD wmWritten; 

	ReadFile(hFile, (char*)&agent.fitness, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&agent.distance, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&agent.activity, (DWORD)sizeof(double), &wmWritten,NULL);

	if(cur_program_version >= 2)
	{
		ReadFile(hFile, (char*)&agent.netwConsistency, (DWORD)sizeof(double), &wmWritten,NULL);
	}

	int num;
	ReadFile(hFile, (char*)&num, (DWORD)sizeof(int), &wmWritten,NULL);
    
	int con_num=0;
	agent.neurons.deleteNeurons();
	for(int i=0; i<num; i++)
	{
		agent.neurons.neuronsPtrs.push_back(NeuronFactory::createNeuron(agent.neurons.networkParams.neuronType));
		ReadFile(hFile,(char*)&con_num, (DWORD)sizeof(int), &wmWritten,NULL);
		agent.neurons[i].connections.resize(con_num);

		ReadFile(hFile,(char*)&agent.neurons[i].id, (DWORD)sizeof(int), &wmWritten,NULL);
		agent.neurons[i].prevId = agent.neurons[i].id;
		ReadFile(hFile,(char*)&agent.neurons[i].type, (DWORD)sizeof(int), &wmWritten,NULL);
	
		//neuron params
		NeuronParams& np =  agent.neurons[i].neuronParams;

		ReadFile(hFile,(char*)&np.useARConnections,(DWORD)sizeof(bool), &wmWritten,NULL);
		ReadFile(hFile,(char*)&np.useNormalization, (DWORD)sizeof(bool), &wmWritten,NULL);
		ReadFile(hFile,(char*)&np.useRandomAddition, (DWORD)sizeof(bool), &wmWritten,NULL);

		ReadFile(hFile,(char*)&np.isIncreasing, (DWORD)sizeof(bool), &wmWritten,NULL);

		ReadFile(hFile,(char*)&np.jNum, (DWORD)sizeof(int), &wmWritten,NULL);
		ReadFile(hFile,(char*)&np.transmissionFuncType, (DWORD)sizeof(int), &wmWritten,NULL);

		ReadFile(hFile,(char*)&np.delayTime,(DWORD)sizeof(double), &wmWritten,NULL);
		ReadFile(hFile,(char*)&np.threshold, (DWORD)sizeof(double), &wmWritten,NULL);
		ReadFile(hFile,(char*)&np.resThreshold, (DWORD)sizeof(double), &wmWritten,NULL);
		ReadFile(hFile,(char*)&np.logCoeff, (DWORD)sizeof(double), &wmWritten,NULL);
		ReadFile(hFile,(char*)&np.uRand,(DWORD)sizeof(double), &wmWritten,NULL);
		ReadFile(hFile,(char*)&np.aRand,(DWORD)sizeof(double), &wmWritten,NULL);

		ReadFile(hFile,(char*)&np.minPotential,(DWORD)sizeof(double), &wmWritten,NULL);
		ReadFile(hFile,(char*)&np.maxPotential,(DWORD)sizeof(double), &wmWritten,NULL);
		ReadFile(hFile,(char*)&np.frequency,(DWORD)sizeof(double), &wmWritten,NULL);

		ReadFile(hFile,(char*)&np.lBound,(DWORD)sizeof(double), &wmWritten,NULL);
		ReadFile(hFile,(char*)&np.rBound,(DWORD)sizeof(double), &wmWritten,NULL);
		ReadFile(hFile,(char*)&np.boundVal,(DWORD)sizeof(double), &wmWritten,NULL);

		ReadFile(hFile,(char*)&np.delayNum,(DWORD)sizeof(int), &wmWritten,NULL);

		//connections
		for(int j=0; j<con_num; j++)
		{
			Connection& c = agent.neurons[i].connections[j]; 

			ReadFile(hFile,(char*)&c.typeOfConnection, (DWORD)sizeof(int), &wmWritten,NULL);
			ReadFile(hFile,(char*)&c.typeOfNeuron, (DWORD)sizeof(int), &wmWritten,NULL);
			ReadFile(hFile,(char*)&c.neuronId,  (DWORD)sizeof(int), &wmWritten,NULL);
			ReadFile(hFile,(char*)&c.weight,(DWORD)sizeof(double), &wmWritten,NULL);
		}
	}
}

void AgentUtil::saveDefaultParams()
{
	HANDLE hFile; 
	hFile = CreateFile("default.prm", GENERIC_READ|GENERIC_WRITE, 
	FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL); 

    saveParamsToFile(hFile);
	CloseHandle(hFile); 
}

bool AgentUtil::loadDefaultParams()
{
	HANDLE hFile; 
	hFile = CreateFile("default.prm", GENERIC_READ, 
	FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL); 
	if (INVALID_HANDLE_VALUE != hFile)
	{
		loadParamsFromFile(hFile);
		CloseHandle(hFile);
		return true;
	}
	
	return false;
}

void AgentUtil::saveParamsToFile(string file_name)
{
	HANDLE hFile; 
	DWORD wmWritten; 
	hFile = CreateFile(file_name.data(), GENERIC_READ|GENERIC_WRITE, 
	FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL); 

	saveParamsToFile(hFile);

	CloseHandle(hFile); 
}

void AgentUtil::loadParamsFromFile(string file_name)
{
	HANDLE hFile; 
	DWORD wmWritten; 
	hFile = CreateFileA(file_name.data(),GENERIC_READ|GENERIC_WRITE, 
	FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL); 

	loadParamsFromFile(hFile);

	CloseHandle(hFile);
}

void AgentUtil::saveParamsToFile(HANDLE& hFile)
{
	DWORD wmWritten; 

	WriteFile(hFile, (char*)&Parameters::programVersion, (DWORD)sizeof(int), &wmWritten,NULL);

	WriteFile(hFile, (char*)&Parameters::JOINT_NUM, (DWORD)sizeof(int), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::OUTPUT_NUM, (DWORD)sizeof(int), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::ALL_SPACE_DIM, (DWORD)sizeof(int), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::networkType, (DWORD)sizeof(int), &wmWritten,NULL);

	WriteFile(hFile, (char*)&Parameters::GENERATION_NUM, (DWORD)sizeof(int), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::DESCENDANT_NUM, (DWORD)sizeof(int), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::ONE_AGENT_LIFE_TIME, (DWORD)sizeof(int), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::typeOfFitness, (DWORD)sizeof(int), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::typeOfControl, (DWORD)sizeof(int), &wmWritten,NULL);

	WriteFile(hFile, (char*)&Parameters::ANGLE_STEP_SIZE, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::dimentionOfInput, (DWORD)sizeof(int), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::PROCCESSES_NUM, (DWORD)sizeof(int), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::NAO_NUM_IN_SCENE, (DWORD)sizeof(int), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::SAVE_GENERATION_NUM, (DWORD)sizeof(int), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::SEED, (DWORD)sizeof(int), &wmWritten,NULL);

	WriteFile(hFile, (char*)&Parameters::LAUNCH_NUM, (DWORD)sizeof(int), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::autoloadParams, (DWORD)sizeof(bool), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::autosaveNetworks, (DWORD)sizeof(bool), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::autosaveParams, (DWORD)sizeof(bool), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::autosaveStatistic, (DWORD)sizeof(bool), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::showJointsData, (DWORD)sizeof(bool), &wmWritten,NULL);

	WriteFile(hFile, (char*)&Parameters::neuronType, (DWORD)sizeof(int), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::inputTransmissionFuncType, (DWORD)sizeof(int), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::intermTransmissionFuncType, (DWORD)sizeof(int), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::outTransmissionFuncType, (DWORD)sizeof(int), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::LOG_FUNC_LINEAR_MAX, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::LOG_FUNC_K_COEFF, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::useNormalization, (DWORD)sizeof(bool), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::useRandomAddition, (DWORD)sizeof(bool), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::MAX_RANDOM_WEIGHT, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::MAX_POTENTIAL, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::MIN_POTENTIAL, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::ACTIVATION_THR, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::GAUSSE_BOUND_VALUE, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::A_RAND, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::U_RAND, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::RESULT_THR, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::NEURON_DELAY_TIME, (DWORD)sizeof(int), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::OUT_MILTIPLY_COEFF, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::SIN_FREQUENCY, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::START_INTERM_NEURON_NUM, (DWORD)sizeof(int), &wmWritten,NULL);

	WriteFile(hFile, (char*)&Parameters::useReadyNetwork, (DWORD)sizeof(bool), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::readyNetworkType, (DWORD)sizeof(int), &wmWritten,NULL);
	//200 max size of neuron matrix
	WriteFile(hFile, Parameters::neuronsMatrix.data(), (DWORD)200, &wmWritten,NULL);

	WriteFile(hFile, (char*)&Parameters::FULLY_NETW_NEURON_NUM, (DWORD)sizeof(int), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::INPUT_DELAY_NUM, (DWORD)sizeof(int), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::BACK_FROM_OUTPUT_DELAY_NUM, (DWORD)sizeof(int), &wmWritten,NULL);

	WriteFile(hFile, (char*)&Parameters::evolutionAlgorithmType, (DWORD)sizeof(int), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::descendantCreationType, (DWORD)sizeof(int), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::selectionType, (DWORD)sizeof(int), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::selectFromStable, (DWORD)sizeof(bool), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::mutateStablePartOfPopulation, (DWORD)sizeof(bool), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::PORTION_OF_CHILDS, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::P_MUTATION_WEIGHT, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::P_MUTATION_THRESHOLD, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::P_MUTATION_RES_THRESHOLD, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::P_MUTATION_TIME, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::P_MUTATION_FREQUENCY, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::S_WEGHT, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::S_THRESH, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::S_TIME_DELAY, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::S_RES_THRESH, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::S_FREQUENCY, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::P_DUPLICATION, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::P_DELETION, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::P_CONNECTION_ADD, (DWORD)sizeof(double), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::P_CONNECTION_REMOVE, (DWORD)sizeof(double), &wmWritten,NULL);
	
	WriteFile(hFile, (char*)&Parameters::dublicateInputNeurons, (DWORD)sizeof(bool), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::dublicateOutputNeurons, (DWORD)sizeof(bool), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::deleteInputNeurons, (DWORD)sizeof(bool), &wmWritten,NULL);
	WriteFile(hFile, (char*)&Parameters::deleteOutputNeurons, (DWORD)sizeof(bool), &wmWritten,NULL);

	if(Parameters::programVersion >= 1)
	{
		WriteFile(hFile, (char*)&Parameters::addInhibitoryConnectionsToOutput, (DWORD)sizeof(bool), &wmWritten,NULL);
		WriteFile(hFile, (char*)&Parameters::addBackConnectionToOutput, (DWORD)sizeof(bool), &wmWritten,NULL);
	}
}

void AgentUtil::loadParamsFromFile(HANDLE& hFile)
{
	DWORD wmWritten; 

	ReadFile(hFile, (char*)&cur_program_version, (DWORD)sizeof(int), &wmWritten,NULL);

	ReadFile(hFile, (char*)&Parameters::JOINT_NUM, (DWORD)sizeof(int), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::OUTPUT_NUM, (DWORD)sizeof(int), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::ALL_SPACE_DIM, (DWORD)sizeof(int), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::networkType, (DWORD)sizeof(int), &wmWritten,NULL);

	ReadFile(hFile, (char*)&Parameters::GENERATION_NUM, (DWORD)sizeof(int), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::DESCENDANT_NUM, (DWORD)sizeof(int), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::ONE_AGENT_LIFE_TIME, (DWORD)sizeof(int), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::typeOfFitness, (DWORD)sizeof(int), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::typeOfControl, (DWORD)sizeof(int), &wmWritten,NULL);

	ReadFile(hFile, (char*)&Parameters::ANGLE_STEP_SIZE, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::dimentionOfInput, (DWORD)sizeof(int), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::PROCCESSES_NUM, (DWORD)sizeof(int), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::NAO_NUM_IN_SCENE, (DWORD)sizeof(int), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::SAVE_GENERATION_NUM, (DWORD)sizeof(int), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::SEED, (DWORD)sizeof(int), &wmWritten,NULL);

	ReadFile(hFile, (char*)&Parameters::LAUNCH_NUM, (DWORD)sizeof(int), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::autoloadParams, (DWORD)sizeof(bool), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::autosaveNetworks, (DWORD)sizeof(bool), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::autosaveParams, (DWORD)sizeof(bool), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::autosaveStatistic, (DWORD)sizeof(bool), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::showJointsData, (DWORD)sizeof(bool), &wmWritten,NULL);

	ReadFile(hFile, (char*)&Parameters::neuronType, (DWORD)sizeof(int), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::inputTransmissionFuncType, (DWORD)sizeof(int), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::intermTransmissionFuncType, (DWORD)sizeof(int), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::outTransmissionFuncType, (DWORD)sizeof(int), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::LOG_FUNC_LINEAR_MAX, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::LOG_FUNC_K_COEFF, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::useNormalization, (DWORD)sizeof(bool), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::useRandomAddition, (DWORD)sizeof(bool), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::MAX_RANDOM_WEIGHT, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::MAX_POTENTIAL, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::MIN_POTENTIAL, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::ACTIVATION_THR, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::GAUSSE_BOUND_VALUE, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::A_RAND, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::U_RAND, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::RESULT_THR, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::NEURON_DELAY_TIME, (DWORD)sizeof(int), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::OUT_MILTIPLY_COEFF, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::SIN_FREQUENCY, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::START_INTERM_NEURON_NUM, (DWORD)sizeof(int), &wmWritten,NULL);

	ReadFile(hFile, (char*)&Parameters::useReadyNetwork, (DWORD)sizeof(bool), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::readyNetworkType, (DWORD)sizeof(int), &wmWritten,NULL);
	
	char buff[200];
	ReadFile(hFile, buff, (DWORD)200, &wmWritten,NULL);
	Parameters::neuronsMatrix = string(buff);

	ReadFile(hFile, (char*)&Parameters::FULLY_NETW_NEURON_NUM, (DWORD)sizeof(int), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::INPUT_DELAY_NUM, (DWORD)sizeof(int), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::BACK_FROM_OUTPUT_DELAY_NUM, (DWORD)sizeof(int), &wmWritten,NULL);

	ReadFile(hFile, (char*)&Parameters::evolutionAlgorithmType, (DWORD)sizeof(int), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::descendantCreationType, (DWORD)sizeof(int), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::selectionType, (DWORD)sizeof(int), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::selectFromStable, (DWORD)sizeof(bool), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::mutateStablePartOfPopulation, (DWORD)sizeof(bool), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::PORTION_OF_CHILDS, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::P_MUTATION_WEIGHT, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::P_MUTATION_THRESHOLD, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::P_MUTATION_RES_THRESHOLD, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::P_MUTATION_TIME, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::P_MUTATION_FREQUENCY, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::S_WEGHT, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::S_THRESH, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::S_TIME_DELAY, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::S_RES_THRESH, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::S_FREQUENCY, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::P_DUPLICATION, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::P_DELETION, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::P_CONNECTION_ADD, (DWORD)sizeof(double), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::P_CONNECTION_REMOVE, (DWORD)sizeof(double), &wmWritten,NULL);
	
	ReadFile(hFile, (char*)&Parameters::dublicateInputNeurons, (DWORD)sizeof(bool), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::dublicateOutputNeurons, (DWORD)sizeof(bool), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::deleteInputNeurons, (DWORD)sizeof(bool), &wmWritten,NULL);
	ReadFile(hFile, (char*)&Parameters::deleteOutputNeurons, (DWORD)sizeof(bool), &wmWritten,NULL); 

	if(cur_program_version >= 1)
	{
		ReadFile(hFile, (char*)&Parameters::addInhibitoryConnectionsToOutput, (DWORD)sizeof(bool), &wmWritten,NULL);
		ReadFile(hFile, (char*)&Parameters::addBackConnectionToOutput, (DWORD)sizeof(bool), &wmWritten,NULL);
	}
}

void AgentUtil::saveResults(string file_name, int programVersion)
{
	HANDLE hFile; 
	DWORD wmWritten; 
	hFile = CreateFile(file_name.data(), GENERIC_READ|GENERIC_WRITE, 
	FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL); 

	WriteFile(hFile, (char*)&programVersion, (DWORD)sizeof(int), &wmWritten,NULL);

	int size = curStatistic.avFitness.size();
	WriteFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
	for(int i=0; i< size; ++i)
		WriteFile(hFile, (char*)&curStatistic.avFitness[i], (DWORD)sizeof(double), &wmWritten,NULL);

	size = curStatistic.maxFitness.size();
	WriteFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
	for(int i=0; i< size; ++i)
		WriteFile(hFile, (char*)&curStatistic.maxFitness[i], (DWORD)sizeof(double), &wmWritten,NULL);

	size = curStatistic.maxNumOfNeurons.size();
	WriteFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
	for(int i=0; i< size; ++i)
		WriteFile(hFile, (char*)&curStatistic.maxNumOfNeurons[i], (DWORD)sizeof(double), &wmWritten,NULL);

	size = curStatistic.minNumOfNeurons.size();
	WriteFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
	for(int i=0; i< size; ++i)
		WriteFile(hFile, (char*)&curStatistic.minNumOfNeurons[i], (DWORD)sizeof(double), &wmWritten,NULL);

	size = curStatistic.avNumOfNeurons.size();
	WriteFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
	for(int i=0; i< size; ++i)
		WriteFile(hFile, (char*)&curStatistic.avNumOfNeurons[i], (DWORD)sizeof(double), &wmWritten,NULL);

	size = curStatistic.maxNumOfConnections.size();
	WriteFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
	for(int i=0; i< size; ++i)
		WriteFile(hFile, (char*)&curStatistic.maxNumOfConnections[i], (DWORD)sizeof(double), &wmWritten,NULL);

	size = curStatistic.minNumOfConnections.size();
	WriteFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
	for(int i=0; i< size; ++i)
		WriteFile(hFile, (char*)&curStatistic.minNumOfConnections[i], (DWORD)sizeof(double), &wmWritten,NULL);

	size = curStatistic.avNumOfConnections.size();
	WriteFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
	for(int i=0; i< size; ++i)
		WriteFile(hFile, (char*)&curStatistic.avNumOfConnections[i], (DWORD)sizeof(double), &wmWritten,NULL);

	if(programVersion >= 2)
	{
		size = curStatistic.bestAgentAvDelayTime.size();
		WriteFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
		for(int i=0; i< size; ++i)
			WriteFile(hFile, (char*)&curStatistic.bestAgentAvDelayTime[i], (DWORD)sizeof(double), &wmWritten,NULL);

		size = curStatistic.dispersOfBestAgentAvDelayTime.size();
		WriteFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
		for(int i=0; i< size; ++i)
			WriteFile(hFile, (char*)&curStatistic.dispersOfBestAgentAvDelayTime[i], (DWORD)sizeof(double), &wmWritten,NULL);

		size = curStatistic.averageAvDelayTime.size();
		WriteFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
		for(int i=0; i< size; ++i)
			WriteFile(hFile, (char*)&curStatistic.averageAvDelayTime[i], (DWORD)sizeof(double), &wmWritten,NULL);

		size = curStatistic.dispersOfAverageAvDelayTime.size();
		WriteFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
		for(int i=0; i< size; ++i)
			WriteFile(hFile, (char*)&curStatistic.dispersOfAverageAvDelayTime[i], (DWORD)sizeof(double), &wmWritten,NULL);
	}

	if(programVersion >= 3)
	{
		size = curStatistic.bestAgentNetConsistency.size();
		WriteFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
		for(int i=0; i< size; ++i)
			WriteFile(hFile, (char*)&curStatistic.bestAgentNetConsistency[i], (DWORD)sizeof(double), &wmWritten,NULL);

		size = curStatistic.avNetConsistency.size();
		WriteFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
		for(int i=0; i< size; ++i)
			WriteFile(hFile, (char*)&curStatistic.avNetConsistency[i], (DWORD)sizeof(double), &wmWritten,NULL);

		size = curStatistic.bestAgentResultTime.size();
		WriteFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
		for(int i=0; i< size; ++i)
			WriteFile(hFile, (char*)&curStatistic.bestAgentResultTime[i], (DWORD)sizeof(double), &wmWritten,NULL);

		size = curStatistic.avResultTime.size();
		WriteFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
		for(int i=0; i< size; ++i)
			WriteFile(hFile, (char*)&curStatistic.avResultTime[i], (DWORD)sizeof(double), &wmWritten,NULL);
	}

	CloseHandle(hFile); 
}

#define PRINT_STAT(paramName)\
	for(int i=0; i<lastStat.paramName.size(); ++i){\
		ostringstream oss;\
		string s;\
		size_t pos=0;\
		oss << lastStat.paramName[i];\
		s = oss.str();\
		pos = s.find_first_of('.');\
		s.replace(pos,pos+1,",");\
		os<<s<<";";\
	}\


void AgentUtil::saveFromStatistic(int curSelection){
	
	if(loadedStatistcis.size() < 1){
		MessageBox(NULL,"Загрузите статистику!","Ошибка",NULL);
		return;
	}

	ofstream os("save.txt");

	Statistic lastStat = loadedStatistcis[loadedStatistcis.size()-1];

	if(curSelection == Constants::MAX_FITNESS_PLOT){
		PRINT_STAT(maxFitness);
	}else if(curSelection == Constants::AVERAGE_FITNESS_PLOT){
		PRINT_STAT(avFitness);
	}else if(curSelection == Constants::MAX_NEURONS_PLOT){
		PRINT_STAT(maxNumOfNeurons);
	}else if(curSelection == Constants::MIN_NEURONS_PLOT){
		PRINT_STAT(minNumOfNeurons);
	}else if(curSelection == Constants::AVERAGE_NEURONS_PLOT){
		PRINT_STAT(avNumOfNeurons);
	}else if(curSelection == Constants::MAX_CONNECTIONS_PLOT){
		PRINT_STAT(maxNumOfConnections);
	}else if(curSelection == Constants::MIN_CONNECTIONS_PLOT){
		PRINT_STAT(minNumOfConnections);
	}else if(curSelection == Constants::AVERAGE_CONNECTIONS_PLOT){
		PRINT_STAT(avNumOfConnections);
	}else if(curSelection == Constants::BEST_AGENT_AV_DELAY_TIME){
		PRINT_STAT(bestAgentAvDelayTime);
	}else if(curSelection == Constants::DISPERS_BEST_AGENT_AV_DELAY_TIME){
		PRINT_STAT(dispersOfBestAgentAvDelayTime);
	}else if(curSelection == Constants::AVERAGE_AV_DELAY_TIME){
		PRINT_STAT(averageAvDelayTime);
	}else if(curSelection == Constants::DISPERS_OF_AVAREGE_AV_DELAY_TIME){
		PRINT_STAT(dispersOfAverageAvDelayTime);
	}else if(curSelection == Constants::BEST_AGENT_NET_CONSISTENSY_PLOT){
		PRINT_STAT(bestAgentNetConsistency);
	}else if(curSelection == Constants::AVERAGE_NET_CONSISTENSY_PLOT){
		PRINT_STAT(avNetConsistency);
	}else if(curSelection == Constants::BEST_AGENT_RESULT_TIME_PLOT){
		PRINT_STAT(bestAgentResultTime);
	}else if(curSelection == Constants::AVERAGE_RESULT_TIME_PLOT){
		PRINT_STAT(avResultTime);
	}

	MessageBox(NULL,"Данные сохранены!","Все ок",NULL);
	os.close();
}

#define PRINT_NET(paramName)\
	for(int i=0; i<g.descendances.size(); ++i){\
		ostringstream oss;\
		string s;\
		size_t pos=0;\
		oss << g.descendances[i].paramName;\
		s = oss.str();\
		pos = s.find_first_of('.');\
		if(pos != string::npos)\
			s.replace(pos,1,",");\
		os<<s<<"\n";\
	}\

#define PRINT_NET_FUNC(paramName)\
	for(int i=0; i<g.descendances.size(); ++i){\
		ostringstream oss;\
		string s;\
		size_t pos=0;\
		oss << g.descendances[i].neurons.paramName();\
		s = oss.str();\
		pos = s.find_first_of('.');\
		if(pos != string::npos)\
			s.replace(pos,1,",");\
		os<<s<<"\n";\
	}\

#define PRINT_NET_FUNC_FOR_AGENT(paramName)\
	ostringstream oss;\
	string s;\
	size_t pos=0;\
	oss << agent.neurons.paramName();\
	s = oss.str();\
	pos = s.find_first_of('.');\
	if(pos != string::npos)\
		s.replace(pos,1,",");\
	os<<s<<"\n";\


#define PRINT_FOR_NEURONS(paramName)\
	for(int i=0; i<agent.neurons.size(); ++i){\
		ostringstream oss;\
		string s;\
		size_t pos=0;\
		oss << agent.neurons[i].neuronParams.paramName;\
		s = oss.str();\
		pos = s.find_first_of('.');\
		if(pos != string::npos)\
			s.replace(pos,1,",");\
		os<<s<<"\n";\
	}\

#define PRINT_FOR_AGENT(paramName)\
		ostringstream oss;\
		string s;\
		size_t pos=0;\
		oss << agent.paramName;\
		s = oss.str();\
		pos = s.find_first_of('.');\
		if(pos != string::npos)\
			s.replace(pos,1,",");\
		os<<s<<"\n";\

void AgentUtil::saveFromNetwork(Generation& g, int curSelection, bool forBestAgent){
	if(g.descendances.size() < 1){
		MessageBox(NULL,"Загрузите сеть!","Ошибка",NULL);
		return;
	}
	
	ofstream os("save.txt");

	AgentInfo agent = curStatistic.findBestAgent(g);

	if(curSelection == Constants::FITNESS_SAVE){
		if(forBestAgent){ PRINT_FOR_AGENT(fitness)} else{ PRINT_NET(fitness)};
	}else if(curSelection == Constants::DISTANCE_SAVE){
		if(forBestAgent){ PRINT_FOR_AGENT(distance)} else{ PRINT_NET(distance)};
	}else if(curSelection == Constants::ACTIVITY_SAVE){
		if(forBestAgent){ PRINT_FOR_AGENT(activity)} else{ PRINT_NET(activity)};
	}else if(curSelection == Constants::NET_CONSISTENCY_SAVE){
		if(forBestAgent){ PRINT_FOR_AGENT(netwConsistency)} else{ PRINT_NET(netwConsistency)};
	}else if(curSelection == Constants::NEURONS_NUM_SAVE){
		if(forBestAgent){ PRINT_NET_FUNC_FOR_AGENT(size)} else{ PRINT_NET_FUNC(size)};
	}else if(curSelection == Constants::CONNECTIONS_NUM_SAVE){
		if(forBestAgent){ PRINT_NET_FUNC_FOR_AGENT(connectionNum)} else{ PRINT_NET_FUNC(connectionNum)};
	}else if(curSelection == Constants::MAX_DELAY_SAVE){
		if(forBestAgent){ PRINT_NET_FUNC_FOR_AGENT(getMaxDelay)} else{ PRINT_NET_FUNC(getMaxDelay)};
	}else if(curSelection == Constants::MIN_DELAY_SAVE){
		if(forBestAgent){ PRINT_NET_FUNC_FOR_AGENT(getMinDelay)} else{ PRINT_NET_FUNC(getMinDelay)};
	}else if(curSelection == Constants::AV_DELAY_SAVE){
		if(forBestAgent){ PRINT_NET_FUNC_FOR_AGENT(getAvDelay)} else{ PRINT_NET_FUNC(getAvDelay)};
	}else if(curSelection == Constants::DELAY_FOR_BEST_AG_SAVE){
		PRINT_FOR_NEURONS(delayTime);
	}else if(curSelection == Constants::AV_TIME_OF_ACHIEVING_RESULT_SAVE){
		if(forBestAgent){ PRINT_FOR_AGENT(timeOfAchievingResults)} else{PRINT_NET(timeOfAchievingResults);};
	}else if(curSelection == Constants::ACTIVATION_CONNECTION_NUM_SAVE){
		if(forBestAgent){ PRINT_NET_FUNC_FOR_AGENT(activateConnectionNum)} else{ PRINT_NET_FUNC(activateConnectionNum)};
	}else if(curSelection == Constants::AR_CONNECTION_NUM_SAVE){
		if(forBestAgent){ PRINT_NET_FUNC_FOR_AGENT(arConnectionNum)} else{ PRINT_NET_FUNC(arConnectionNum)};
	}

	MessageBox(NULL,"Данные сохранены!","Все ок",NULL);
	os.close();
}

void AgentUtil::calcAvarageStat(Statistic& stat){
	if(loadedStatistcis.size() == 0) return;

	stat.clear();
	stat = loadedStatistcis[0];
	for(int i=1; i<loadedStatistcis.size();++i)
		stat += loadedStatistcis[i];
	stat /= loadedStatistcis.size();
}

void AgentUtil::saveAvStatistic(string file_name){
	calcAvarageStat(curStatistic);
	saveResults(file_name, cur_program_version);
}

void AgentUtil::loadResults(string file_name)
{
	if(clearStatBuffer)
		loadedStatistcis.resize(1);
	else 
		loadedStatistcis.resize(loadedStatistcis.size() + 1);

	Statistic& lastStat = loadedStatistcis[loadedStatistcis.size() - 1];
	lastStat.clear();

	HANDLE hFile; 
	DWORD wmWritten; 
	hFile = CreateFile(file_name.data(), GENERIC_READ|GENERIC_WRITE, 
	FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL); 

	ReadFile(hFile, (char*)&AgentUtil::cur_program_version, (DWORD)sizeof(int), &wmWritten,NULL);

	int size;
	ReadFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
	lastStat.avFitness.resize(size);
	for(int i=0; i< size; ++i)
		ReadFile(hFile, (char*)&lastStat.avFitness[i], (DWORD)sizeof(double), &wmWritten,NULL);

	ReadFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
	lastStat.maxFitness.resize(size);
	for(int i=0; i< size; ++i)
		ReadFile(hFile, (char*)&lastStat.maxFitness[i], (DWORD)sizeof(double), &wmWritten,NULL);

	ReadFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
	lastStat.maxNumOfNeurons.resize(size);
	for(int i=0; i< size; ++i)
		ReadFile(hFile, (char*)&lastStat.maxNumOfNeurons[i], (DWORD)sizeof(double), &wmWritten,NULL);

	ReadFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
	lastStat.minNumOfNeurons.resize(size);
	for(int i=0; i< size; ++i)
		ReadFile(hFile, (char*)&lastStat.minNumOfNeurons[i], (DWORD)sizeof(double), &wmWritten,NULL);

	ReadFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
	lastStat.avNumOfNeurons.resize(size);
	for(int i=0; i< size; ++i)
		ReadFile(hFile, (char*)&lastStat.avNumOfNeurons[i], (DWORD)sizeof(double), &wmWritten,NULL);

	ReadFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
	lastStat.maxNumOfConnections.resize(size);
	for(int i=0; i< size; ++i)
		ReadFile(hFile, (char*)&lastStat.maxNumOfConnections[i], (DWORD)sizeof(double), &wmWritten,NULL);

	ReadFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
	lastStat.minNumOfConnections.resize(size);
	for(int i=0; i< size; ++i)
		ReadFile(hFile, (char*)&lastStat.minNumOfConnections[i], (DWORD)sizeof(double), &wmWritten,NULL);

	ReadFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
	lastStat.avNumOfConnections.resize(size);
	for(int i=0; i< size; ++i)
		ReadFile(hFile, (char*)&lastStat.avNumOfConnections[i], (DWORD)sizeof(double), &wmWritten,NULL);

	if(Parameters::programVersion >= 2)
	{
		ReadFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
		lastStat.bestAgentAvDelayTime.resize(size);
		for(int i=0; i< size; ++i)
			ReadFile(hFile, (char*)&lastStat.bestAgentAvDelayTime[i], (DWORD)sizeof(double), &wmWritten,NULL);

		ReadFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
		lastStat.dispersOfBestAgentAvDelayTime.resize(size);
		for(int i=0; i< size; ++i)
			ReadFile(hFile, (char*)&lastStat.dispersOfBestAgentAvDelayTime[i], (DWORD)sizeof(double), &wmWritten,NULL);

		ReadFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
		lastStat.averageAvDelayTime.resize(size);
		for(int i=0; i< size; ++i)
			ReadFile(hFile, (char*)&lastStat.averageAvDelayTime[i], (DWORD)sizeof(double), &wmWritten,NULL);

		ReadFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
		lastStat.dispersOfAverageAvDelayTime.resize(size);
		for(int i=0; i< size; ++i)
			ReadFile(hFile, (char*)&lastStat.dispersOfAverageAvDelayTime[i], (DWORD)sizeof(double), &wmWritten,NULL);
	}

	if(Parameters::programVersion >= 3)
	{
		ReadFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
		lastStat.bestAgentNetConsistency.resize(size);
		for(int i=0; i< size; ++i)
			ReadFile(hFile, (char*)&lastStat.bestAgentNetConsistency[i], (DWORD)sizeof(double), &wmWritten,NULL);

		ReadFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
		lastStat.avNetConsistency.resize(size);
		for(int i=0; i< size; ++i)
			ReadFile(hFile, (char*)&lastStat.avNetConsistency[i], (DWORD)sizeof(double), &wmWritten,NULL);

		ReadFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
		lastStat.bestAgentResultTime.resize(size);
		for(int i=0; i< size; ++i)
			ReadFile(hFile, (char*)&lastStat.bestAgentResultTime[i], (DWORD)sizeof(double), &wmWritten,NULL);

		ReadFile(hFile, (char*)&size, (DWORD)sizeof(int), &wmWritten,NULL);
		lastStat.avResultTime.resize(size);
		for(int i=0; i< size; ++i)
			ReadFile(hFile, (char*)&lastStat.avResultTime[i], (DWORD)sizeof(double), &wmWritten,NULL);
	}

	CloseHandle(hFile);
}

void AgentUtil::saveRuntimeInfo(Generation& g)
{
	writeResultsInFile(g);
	if(Parameters::autosaveStatistic)
		AgentUtil::saveResults("statistic_"+Util::to_string(cur_lunch)+".rslt", Parameters::programVersion);
	if(Parameters::autosaveNetworks)
		AgentUtil::saveAgentsToFile(g, "networks_"+Util::to_string(cur_lunch)+".agnt");
}

bool AgentUtil::checkAgentIsActive(int agentId)
{
	return cur_session*processes_num + agentId < Parameters::DESCENDANT_NUM;
}

bool AgentUtil::hasNextSession()
{
	return (cur_session < robots_num_in_process - 1);
}

void AgentUtil::setNextSession()
{
	cur_session++;
}


void AgentUtil::sendMessagesToAgents(int command)
{
	if(command == Constants::INITIALIZE_CYCLE_MESSAGE)
	{
		for(int i=0; i<processes_num; i++)
		{
			SetEvent(hEvtsReadNetworkAndReset[i]);
		}
	}
	else if(command == Constants::START_LIFE_MESSAGE)
	{
		for(int i=0; i<processes_num; i++)
		{
			SetEvent(hEvtsStartLife[i]);
		}
	}
	else if(command == Constants::EXIT_PROCESS_MESSAGE)
	{
		for(int i=0; i<processes_num; i++)
		{
			SetEvent(hEvtsExitProcess[i]);
			//EnumThreadWindows(pi[i].dwThreadId, closeEnumProc, 0); 
		}
	}
}

BOOL CALLBACK AgentUtil::closeEnumProc(HWND hwnd, LPARAM lParam) 
{ 
   SendMessage(hwnd, WM_CLOSE, 0, 0); 
   //SendMessage(hwnd, WM_QUIT, 0, 0); 
   return TRUE; 
} 

void AgentUtil::waitForMessagesFromAgents(int mess)
{
	if(mess == Constants::AGENT_IS_READY_MESSAGE)
	{
		HANDLE* evnts = new HANDLE[processes_num];
		for(int i=0; i<processes_num; i++)
			evnts[i] = hEvtsAgentIsReady[i];

		DWORD dw = WaitForMultipleObjects(processes_num,evnts,TRUE,INFINITE);

		if(dw == WAIT_OBJECT_0)
		{
			//reset events
			for(int i=0; i<processes_num;i++)
				ResetEvent(hEvtsAgentIsReady[i]);
		}
		delete[] evnts;

		return;
	}
	else if(mess == Constants::AGENT_LIFE_ENDED_MESSAGE)
	{
		HANDLE* evnts = new HANDLE[processes_num];
		for(int i=0; i<processes_num; i++)
			evnts[i] = hEvtsAgentLifeEnded[i];

		DWORD dw = WaitForMultipleObjects(processes_num,evnts,TRUE,INFINITE);

		if(dw == WAIT_OBJECT_0)
		{
			//reset events
			for(int i=0; i<processes_num;i++)
				ResetEvent(hEvtsAgentLifeEnded[i]);
		}

		delete[] evnts;
		return;
	}
	else if(mess == Constants::AGENT_PROCESS_CLEARED_MESSAGE)
	{
		HANDLE* evnts = new HANDLE[processes_num];
		for(int i=0; i<processes_num; i++)
			evnts[i] = hEvtsAgetnProcessCleared[i];

		DWORD dw = WaitForMultipleObjects(processes_num,evnts,TRUE,INFINITE);

		if(dw == WAIT_OBJECT_0)
		{
			//reset events
			for(int i=0; i<processes_num;i++)
				ResetEvent(hEvtsAgetnProcessCleared[i]);

		}

		delete[] evnts;
		return;
	}
}

char* AgentUtil::sendMessageToShowAgent(int mess, int& size)
{
	char* p = (char*) pArray_show;
	writeInt(p, mess, size);
	return p;
}

void AgentUtil::writeResultsInFile(Generation& g)
{
	LOG::startFileInput("log.txt");

	if(g.curGeneration != Parameters::GENERATION_NUM - 1)
	{
		LOG::writeMessageInFile("______________________________");
		LOG::writeMessageInFile("generationNum:");
		LOG::writeParamInFile(g.curGeneration);
		LOG::writeMessageInFile("maxFitneses:");
		LOG::writeParamInFile(curStatistic.calcMaxFitness(g));
		LOG::writeMessageInFile("averageFitness:");
		LOG::writeParamInFile(curStatistic.calcAverageFitness(g));
		LOG::writeMessageInFile("max num of interm neurons:");
		LOG::writeParamInFile(curStatistic.calcMaxNumOfNeurons(g));
		LOG::writeMessageInFile("min num of interm neurons:");
		LOG::writeParamInFile(curStatistic.calcMinNumOfNeurons(g));
		LOG::writeMessageInFile("average num of interm neurons:");
		LOG::writeParamInFile(curStatistic.calcAvarageNumOfNeurons(g));
		LOG::writeMessageInFile("average num of interm neurons:");
		LOG::writeParamInFile(curStatistic.calcAvarageNumOfNeurons(g));
		LOG::writeMessageInFile("max num of connections:");
		LOG::writeParamInFile(curStatistic.calcMaxNumOfConnections(g));
		LOG::writeMessageInFile("min num of connections:");
		LOG::writeParamInFile(curStatistic.calcMinNumOfConnections(g));
		LOG::writeMessageInFile("av num of connections:");
		LOG::writeParamInFile(curStatistic.calcAvarageNumOfConnections(g));
	}
	else
	{
		//for the last generation out for all descendant
		LOG::writeMessageInFile("______________________________");
		LOG::writeMessageInFile("generation num");
		LOG::writeParamInFile(g.curGeneration);
		LOG::writeMessageInFile("maxFitneses:");
		LOG::writeArrInFile(curStatistic.maxFitness);
		LOG::writeMessageInFile("averageFitness:");
		LOG::writeArrInFile(curStatistic.avFitness);
		LOG::writeMessageInFile("max num of interm neurons:");
		LOG::writeArrInFile(curStatistic.maxNumOfNeurons);
		LOG::writeMessageInFile("min num of interm neurons:");
		LOG::writeArrInFile(curStatistic.minNumOfNeurons);
		LOG::writeMessageInFile("average num of interm neurons:");
		LOG::writeArrInFile(curStatistic.avNumOfNeurons);
		LOG::writeMessageInFile("max num of connections:");
		LOG::writeArrInFile(curStatistic.maxNumOfConnections);
		LOG::writeMessageInFile("min num of connections:");
		LOG::writeArrInFile(curStatistic.minNumOfConnections);
		LOG::writeMessageInFile("av num of connections:");
		LOG::writeArrInFile(curStatistic.avNumOfConnections);
	}

	LOG::endFileInput();
}

void AgentUtil::showBestAgent(Generation& g, HWND hwnd)
{
	if(g.descendances.size() > 0)
	{
		AgentUtil::createShowAgentProcess(hwnd);
		int size = 0;
		char *p = AgentUtil::sendMessageToShowAgent(-1,size);
		//AgentUtil::writeDescendantInFile(p,size, g.descendances[curStatistic.sortedAgents[0]]);
		AgentUtil::writeDescendantInFile(p,size, g.descendances[0]);
		p = AgentUtil::sendMessageToShowAgent(Constants::READ_NETWORK_AND_PLAY_SHOW_MESS,size);
	}
}

void AgentUtil::showNextAgent(Generation& g, HWND hwnd)
{
	if(g.descendances.size() > 0)
	{
		AgentUtil::createShowAgentProcess(hwnd);
		int size = 0;
		char *p = AgentUtil::sendMessageToShowAgent(-1,size);

		if(AgentUtil::cur_show_agent < g.descendances.size() - 1)
			AgentUtil::cur_show_agent++;
		else
			AgentUtil::cur_show_agent = 0;

		//AgentUtil::writeDescendantInFile(p,size, g.descendances[curStatistic.sortedAgents[AgentUtil::cur_show_agent]]);
		AgentUtil::writeDescendantInFile(p,size, g.descendances[AgentUtil::cur_show_agent]);
		p = AgentUtil::sendMessageToShowAgent(Constants::READ_NETWORK_AND_PLAY_SHOW_MESS,size);
	}
}

void AgentUtil::showPrevAgent(Generation& g, HWND hwnd)
{
	if(g.descendances.size() > 0)
	{
		AgentUtil::createShowAgentProcess(hwnd);
		int size = 0;
		char *p = AgentUtil::sendMessageToShowAgent(-1,size);

		if(AgentUtil::cur_show_agent > 1)
			AgentUtil::cur_show_agent--;
		else
			AgentUtil::cur_show_agent = g.descendances.size() - 1;

		//AgentUtil::writeDescendantInFile(p,size, g.descendances[curStatistic.sortedAgents[AgentUtil::cur_show_agent]]);
		AgentUtil::writeDescendantInFile(p,size, g.descendances[AgentUtil::cur_show_agent]);
		p = AgentUtil::sendMessageToShowAgent(Constants::READ_NETWORK_AND_PLAY_SHOW_MESS,size);
	}
}

void AgentUtil::plotGraph(AzGraphCtrl& m_graph, int graphPointType, int descNum)
{
	if(cur_graph_type == Constants::STATISTIC_TYPE_GRAPH)
	{
		if(statsIsLoaded)
		{
			vector<string> lgnds = m_graph.GetLegends();
			plotStatisticGraph(loadedStatistcis, m_graph, graphPointType, cur_graph_selection, &lgnds);
		}
		else 
		{
			vector<Statistic> temp(0);
			temp.push_back(curStatistic);
			plotStatisticGraph(temp, m_graph, graphPointType, cur_graph_selection);
		}
	}
	else if(cur_graph_type == Constants::JOINT_TYPE_GRAPH)
	{
		plotJointDataGraphs(m_graph, graphPointType, descNum, cur_graph_selection);
	}
	else if(cur_graph_type == Constants::NETWORK_TYPE_GRAPH)
	{
		plotOutNeuronDataGraphs(m_graph, graphPointType, descNum, cur_graph_selection);
	}
}

void AgentUtil::renameLegend(int legNum, string name, AzGraphCtrl& m_graph, int graphPointType)
{
	if(loadedStatistcis.size() < 1) return;

	vector<string> lgnds = m_graph.GetLegends();

	if(legNum < 0 || legNum > lgnds.size() - 1) return;
	lgnds[legNum] = name;

	 plotStatisticGraph(loadedStatistcis, m_graph, graphPointType, cur_graph_selection, &lgnds);
}

void  AgentUtil::plotStatisticGraph(vector<Statistic>&st, AzGraphCtrl& m_graph, int graphPointType, int selection, vector<string>* lgnds)
{
	if(st.size() < 1 && st[0].maxFitness.size() < 2) return;

	m_graph.Clear();
	m_graph.SetGraphType(graphPointType);

	vector<string> legends;
	int count = 0;
	double min = 0;
	double max = 0;
	double dy = 0;
	int _count = 0;
	int _min = 0;
	int _max = 0;
	vector<vector<DOUBLE>> dataX;
	vector<vector<DOUBLE>> dataY;
	dataX.resize(st.size());
	dataY.resize(st.size());

	if(statsIsLoaded)
	{
		if(lgnds != NULL)
			legends = *lgnds;

		for(int i=legends.size(); i<st.size(); ++i)
			legends.push_back(Util::to_string(i));
	}
	
	if(selection == Constants::MAX_FITNESS_PLOT)
	{
		count = st[0].maxFitness.size();
		min =  st[0].calcMin(st[0].maxFitness);
		max = st[0].calcMax(st[0].maxFitness);
		
		for(int i=0; i<st.size(); ++i)
		{
			_count = st[i].maxFitness.size();
			_min =  st[i].calcMin(st[i].maxFitness);
			_max = st[i].calcMax(st[i].maxFitness);
			if(_count > count)
				count = _count;
			if(_min < min)
				min = _min;
			if(_max > max)
				max = _max;

			dataX[i].resize(_count); 
			dataY[i].resize(_count); 

			for(int j=0; j<_count; j++) {
				dataX[i][j] = j;
				dataY[i][j] = st[i].maxFitness[j];
			}
		}
		
		m_graph.SetMinMaxX(0, count, FALSE);
		m_graph.SetUnits("Поколение", "Фитнесс", FALSE);
	}
	else if(selection == Constants::AVERAGE_FITNESS_PLOT)
	{
		count = st[0].avFitness.size();
		min =  st[0].calcMin(st[0].avFitness);
		max = st[0].calcMax(st[0].avFitness);
		
		for(int i=0; i<st.size(); ++i)
		{
			_count = st[i].avFitness.size();
			_min =  st[i].calcMin(st[i].avFitness);
			_max = st[i].calcMax(st[i].avFitness);
			if(_count > count)
				count = _count;
			if(_min < min)
				min = _min;
			if(_max > max)
				max = _max;

			dataX[i].resize(_count); 
			dataY[i].resize(_count); 

			for(int j=0; j<_count; j++) {
				dataX[i][j] = j;
				dataY[i][j] = st[i].avFitness[j];
			}
		}
		
		m_graph.SetMinMaxX(0, count, FALSE);
		m_graph.SetUnits("Поколение", "Фитнесс", FALSE);
	}
	else if(selection == Constants::MAX_NEURONS_PLOT)
	{
		count = st[0].maxNumOfNeurons.size();
		min =  st[0].calcMin(st[0].maxNumOfNeurons);
		max = st[0].calcMax(st[0].maxNumOfNeurons);
		
		for(int i=0; i<st.size(); ++i)
		{
			_count = st[i].maxNumOfNeurons.size();
			_min =  st[i].calcMin(st[i].maxNumOfNeurons);
			_max = st[i].calcMax(st[i].maxNumOfNeurons);
			if(_count > count)
				count = _count;
			if(_min < min)
				min = _min;
			if(_max > max)
				max = _max;

			dataX[i].resize(_count); 
			dataY[i].resize(_count); 

			for(int j=0; j<_count; j++) {
				dataX[i][j] = j;
				dataY[i][j] = st[i].maxNumOfNeurons[j];
			}
		}
		
		m_graph.SetMinMaxX(0, count, FALSE);
		m_graph.SetUnits("Поколение", "Кол-во нейронов", FALSE);
	}
	else if(selection == Constants::MIN_NEURONS_PLOT)
	{
		count = st[0].minNumOfNeurons.size();
		min =  st[0].calcMin(st[0].minNumOfNeurons);
		max = st[0].calcMax(st[0].minNumOfNeurons);
		
		for(int i=0; i<st.size(); ++i)
		{
			_count = st[i].minNumOfNeurons.size();
			_min =  st[i].calcMin(st[i].minNumOfNeurons);
			_max = st[i].calcMax(st[i].minNumOfNeurons);
			if(_count > count)
				count = _count;
			if(_min < min)
				min = _min;
			if(_max > max)
				max = _max;

			dataX[i].resize(_count); 
			dataY[i].resize(_count); 

			for(int j=0; j<_count; j++) {
				dataX[i][j] = j;
				dataY[i][j] = st[i].minNumOfNeurons[j];
			}
		}
		
		m_graph.SetMinMaxX(0, count, FALSE);
		m_graph.SetUnits("Поколение", "Кол-во нейронов", FALSE);

	}
	else if(selection == Constants::AVERAGE_NEURONS_PLOT)
	{
		count = st[0].avNumOfNeurons.size();
		min =  st[0].calcMin(st[0].avNumOfNeurons);
		max = st[0].calcMax(st[0].avNumOfNeurons);
		
		for(int i=0; i<st.size(); ++i)
		{
			_count = st[i].avNumOfNeurons.size();
			_min =  st[i].calcMin(st[i].avNumOfNeurons);
			_max = st[i].calcMax(st[i].avNumOfNeurons);
			if(_count > count)
				count = _count;
			if(_min < min)
				min = _min;
			if(_max > max)
				max = _max;

			dataX[i].resize(_count); 
			dataY[i].resize(_count); 

			for(int j=0; j<_count; j++) {
				dataX[i][j] = j;
				dataY[i][j] = st[i].avNumOfNeurons[j];
			}
		}
		
		m_graph.SetMinMaxX(0, count, FALSE);
		m_graph.SetUnits("Поколение", "Кол-во нейронов", FALSE);
	}
	else if(selection == Constants::MIN_CONNECTIONS_PLOT)
	{
		count = st[0].minNumOfConnections.size();
		min =  st[0].calcMin(st[0].minNumOfConnections);
		max = st[0].calcMax(st[0].minNumOfConnections);
		
		for(int i=0; i<st.size(); ++i)
		{
			_count = st[i].minNumOfConnections.size();
			_min =  st[i].calcMin(st[i].minNumOfConnections);
			_max = st[i].calcMax(st[i].minNumOfConnections);
			if(_count > count)
				count = _count;
			if(_min < min)
				min = _min;
			if(_max > max)
				max = _max;

			dataX[i].resize(_count); 
			dataY[i].resize(_count); 

			for(int j=0; j<_count; j++) {
				dataX[i][j] = j;
				dataY[i][j] = st[i].minNumOfConnections[j];
			}
		}
		
		m_graph.SetMinMaxX(0, count, FALSE);
		m_graph.SetUnits("Поколение", "Кол-во связей", FALSE);
	}
	else if(selection == Constants::MAX_CONNECTIONS_PLOT)
	{
		count = st[0].maxNumOfConnections.size();
		min =  st[0].calcMin(st[0].maxNumOfConnections);
		max = st[0].calcMax(st[0].maxNumOfConnections);
		
		for(int i=0; i<st.size(); ++i)
		{
			_count = st[i].maxNumOfConnections.size();
			_min =  st[i].calcMin(st[i].maxNumOfConnections);
			_max = st[i].calcMax(st[i].maxNumOfConnections);
			if(_count > count)
				count = _count;
			if(_min < min)
				min = _min;
			if(_max > max)
				max = _max;

			dataX[i].resize(_count); 
			dataY[i].resize(_count); 

			for(int j=0; j<_count; j++) {
				dataX[i][j] = j;
				dataY[i][j] = st[i].maxNumOfConnections[j];
			}
		}
		
		m_graph.SetMinMaxX(0, count, FALSE);
		m_graph.SetUnits("Поколение", "Кол-во связей", FALSE);
	}
	else if(selection == Constants::AVERAGE_CONNECTIONS_PLOT)
	{
		count = st[0].avNumOfConnections.size();
		min =  st[0].calcMin(st[0].avNumOfConnections);
		max = st[0].calcMax(st[0].avNumOfConnections);
		
		for(int i=0; i<st.size(); ++i)
		{
			_count = st[i].avNumOfConnections.size();
			_min =  st[i].calcMin(st[i].avNumOfConnections);
			_max = st[i].calcMax(st[i].avNumOfConnections);
			if(_count > count)
				count = _count;
			if(_min < min)
				min = _min;
			if(_max > max)
				max = _max;

			dataX[i].resize(_count); 
			dataY[i].resize(_count); 

			for(int j=0; j<_count; j++) {
				dataX[i][j] = j;
				dataY[i][j] = st[i].avNumOfConnections[j];
			}
		}
		
		m_graph.SetMinMaxX(0, count, FALSE);
		m_graph.SetUnits("Поколение", "Кол-во связей", FALSE);
	}
	else if(selection == Constants::BEST_AGENT_NET_CONSISTENSY_PLOT)
	{
		count = st[0].bestAgentNetConsistency.size();
		min =  st[0].calcMin(st[0].bestAgentNetConsistency);
		max = st[0].calcMax(st[0].bestAgentNetConsistency);
		
		for(int i=0; i<st.size(); ++i)
		{
			_count = st[i].bestAgentNetConsistency.size();
			_min =  st[i].calcMin(st[i].bestAgentNetConsistency);
			_max = st[i].calcMax(st[i].bestAgentNetConsistency);
			if(_count > count)
				count = _count;
			if(_min < min)
				min = _min;
			if(_max > max)
				max = _max;

			dataX[i].resize(_count); 
			dataY[i].resize(_count); 

			for(int j=0; j<_count; j++) {
				dataX[i][j] = j;
				dataY[i][j] = st[i].bestAgentNetConsistency[j];
			}
		}
		
		m_graph.SetMinMaxX(0, count, FALSE);
		m_graph.SetUnits("Поколение", "Согласованность", FALSE);
	}
	else if(selection == Constants::AVERAGE_NET_CONSISTENSY_PLOT)
	{
		count = st[0].avNetConsistency.size();
		min =  st[0].calcMin(st[0].avNetConsistency);
		max = st[0].calcMax(st[0].avNetConsistency);
		
		for(int i=0; i<st.size(); ++i)
		{
			_count = st[i].avNetConsistency.size();
			_min =  st[i].calcMin(st[i].avNetConsistency);
			_max = st[i].calcMax(st[i].avNetConsistency);
			if(_count > count)
				count = _count;
			if(_min < min)
				min = _min;
			if(_max > max)
				max = _max;

			dataX[i].resize(_count); 
			dataY[i].resize(_count); 

			for(int j=0; j<_count; j++) {
				dataX[i][j] = j;
				dataY[i][j] = st[i].avNetConsistency[j];
			}
		}
		
		m_graph.SetMinMaxX(0, count, FALSE);
		m_graph.SetUnits("Поколение", "Согласованность", FALSE);
	}
	else if(selection == Constants::BEST_AGENT_RESULT_TIME_PLOT)
	{
		count = st[0].bestAgentResultTime.size();
		min =  st[0].calcMin(st[0].bestAgentResultTime);
		max = st[0].calcMax(st[0].bestAgentResultTime);
		
		for(int i=0; i<st.size(); ++i)
		{
			_count = st[i].bestAgentResultTime.size();
			_min =  st[i].calcMin(st[i].bestAgentResultTime);
			_max = st[i].calcMax(st[i].bestAgentResultTime);
			if(_count > count)
				count = _count;
			if(_min < min)
				min = _min;
			if(_max > max)
				max = _max;

			dataX[i].resize(_count); 
			dataY[i].resize(_count); 

			for(int j=0; j<_count; j++) {
				dataX[i][j] = j;
				dataY[i][j] = st[i].bestAgentResultTime[j];
			}
		}
		
		m_graph.SetMinMaxX(0, count, FALSE);
		m_graph.SetUnits("Поколение", "Время результата", FALSE);
	}
	else if(selection == Constants::AVERAGE_RESULT_TIME_PLOT)
	{
		count = st[0].avResultTime.size();
		min =  st[0].calcMin(st[0].avResultTime);
		max = st[0].calcMax(st[0].avResultTime);
		
		for(int i=0; i<st.size(); ++i)
		{
			_count = st[i].avResultTime.size();
			_min =  st[i].calcMin(st[i].avResultTime);
			_max = st[i].calcMax(st[i].avResultTime);
			if(_count > count)
				count = _count;
			if(_min < min)
				min = _min;
			if(_max > max)
				max = _max;

			dataX[i].resize(_count); 
			dataY[i].resize(_count); 

			for(int j=0; j<_count; j++) {
				dataX[i][j] = j;
				dataY[i][j] = st[i].avResultTime[j];
			}
		}
		
		m_graph.SetMinMaxX(0, count, FALSE);
		m_graph.SetUnits("Поколение", "Время результата", FALSE);
	}
	else if(selection == Constants::BEST_AGENT_AV_DELAY_TIME)
	{
		count = st[0].bestAgentAvDelayTime.size();
		min =  st[0].calcMin(st[0].bestAgentAvDelayTime);
		max = st[0].calcMax(st[0].bestAgentAvDelayTime);
		
		for(int i=0; i<st.size(); ++i)
		{
			_count = st[i].bestAgentAvDelayTime.size();
			_min =  st[i].calcMin(st[i].bestAgentAvDelayTime);
			_max = st[i].calcMax(st[i].bestAgentAvDelayTime);
			if(_count > count)
				count = _count;
			if(_min < min)
				min = _min;
			if(_max > max)
				max = _max;

			dataX[i].resize(_count); 
			dataY[i].resize(_count); 

			for(int j=0; j<_count; j++) {
				dataX[i][j] = j;
				dataY[i][j] = st[i].bestAgentAvDelayTime[j];
			}
		}
		
		m_graph.SetMinMaxX(0, count, FALSE);
		m_graph.SetUnits("Поколение", "Время задержки", FALSE);
	}
	else if(selection == Constants::DISPERS_BEST_AGENT_AV_DELAY_TIME)
	{
		count = st[0].dispersOfBestAgentAvDelayTime.size();
		min =  st[0].calcMin(st[0].dispersOfBestAgentAvDelayTime);
		max = st[0].calcMax(st[0].dispersOfBestAgentAvDelayTime);
		
		for(int i=0; i<st.size(); ++i)
		{
			_count = st[i].dispersOfBestAgentAvDelayTime.size();
			_min =  st[i].calcMin(st[i].dispersOfBestAgentAvDelayTime);
			_max = st[i].calcMax(st[i].dispersOfBestAgentAvDelayTime);
			if(_count > count)
				count = _count;
			if(_min < min)
				min = _min;
			if(_max > max)
				max = _max;

			dataX[i].resize(_count); 
			dataY[i].resize(_count); 

			for(int j=0; j<_count; j++) {
				dataX[i][j] = j;
				dataY[i][j] = st[i].dispersOfBestAgentAvDelayTime[j];
			}
		}
		
		m_graph.SetMinMaxX(0, count, FALSE);
		m_graph.SetUnits("Поколение", "Дисперсия задержки", FALSE);
	}
	else if(selection == Constants::AVERAGE_AV_DELAY_TIME)
	{
		count = st[0].averageAvDelayTime.size();
		min =  st[0].calcMin(st[0].averageAvDelayTime);
		max = st[0].calcMax(st[0].averageAvDelayTime);
		
		for(int i=0; i<st.size(); ++i)
		{
			_count = st[i].averageAvDelayTime.size();
			_min =  st[i].calcMin(st[i].averageAvDelayTime);
			_max = st[i].calcMax(st[i].averageAvDelayTime);
			if(_count > count)
				count = _count;
			if(_min < min)
				min = _min;
			if(_max > max)
				max = _max;

			dataX[i].resize(_count); 
			dataY[i].resize(_count); 

			for(int j=0; j<_count; j++) {
				dataX[i][j] = j;
				dataY[i][j] = st[i].averageAvDelayTime[j];
			}
		}
		
		m_graph.SetMinMaxX(0, count, FALSE);
		m_graph.SetUnits("Поколение", "Время задержки", FALSE);
	}
	else if(selection == Constants::DISPERS_OF_AVAREGE_AV_DELAY_TIME)
	{
		count = st[0].dispersOfAverageAvDelayTime.size();
		min =  st[0].calcMin(st[0].dispersOfAverageAvDelayTime);
		max = st[0].calcMax(st[0].dispersOfAverageAvDelayTime);
		
		for(int i=0; i<st.size(); ++i)
		{
			_count = st[i].dispersOfAverageAvDelayTime.size();
			_min =  st[i].calcMin(st[i].dispersOfAverageAvDelayTime);
			_max = st[i].calcMax(st[i].dispersOfAverageAvDelayTime);
			if(_count > count)
				count = _count;
			if(_min < min)
				min = _min;
			if(_max > max)
				max = _max;

			dataX[i].resize(_count); 
			dataY[i].resize(_count); 

			for(int j=0; j<_count; j++) {
				dataX[i][j] = j;
				dataY[i][j] = st[i].dispersOfAverageAvDelayTime[j];
			}
		}
		
		m_graph.SetMinMaxX(0, count, FALSE);
		m_graph.SetUnits("Поколение", "Дисперсия задержки", FALSE);
	}

	if(count > 1)
	{
		dy = (max - min)/2;
		min -= dy; if(min < 0) min = 0;
		max += dy;
		if(min == max){ min -= min/10;max += max/10; }

		m_graph.SetMinMaxY(floor(min), ceil(max), FALSE);
		m_graph.SetCaliber(2, FALSE);
		m_graph.SetLegends(legends);
		m_graph.Update();

		m_graph.SetAllData(dataX, dataY);
	}
}

void AgentUtil::plotJointDataGraphs(AzGraphCtrl& m_graph, int pointGraphType, int descNum, int jointNum)
{
	if(curStatistic.jointValues.size() == 0) return;

	m_graph.Clear();
	m_graph.SetGraphType(pointGraphType);

	int count = 0;
	double min = 0;
	double max = 0;
	double dy = 0;

	vector<vector<DOUBLE>> dataX(1);
	vector<vector<DOUBLE>> dataY(1);

	count = curStatistic.jointValues[descNum][jointNum].size();
	m_graph.SetMinMaxX(0, Parameters::ONE_AGENT_LIFE_TIME, FALSE);
	m_graph.SetUnits("время, мс", "Угол сочленения", FALSE);

	min = curStatistic.calcMin(curStatistic.jointValues[descNum][jointNum]);
	max = curStatistic.calcMax(curStatistic.jointValues[descNum][jointNum]);

	dataX[0].resize(count);
	dataY[0].resize(count);

	for(int i=0; i<count; i++) {
		dataX[0][i] = i*Parameters::ONE_AGENT_LIFE_TIME/60;;
		dataY[0][i] = curStatistic.jointValues[descNum][jointNum][i];
	}

	m_graph.SetMinMaxY(floor(min), ceil(max), FALSE);
	m_graph.SetCaliber(2, FALSE);
	m_graph.Update();

	m_graph.SetAllData(dataX, dataY);
}

void AgentUtil::plotOutNeuronDataGraphs(AzGraphCtrl& m_graph, int graphPointType, int descNum, int jointNum)
{
	if(curStatistic.outNetwValuesValues.size() == 0) return;

	m_graph.Clear();
	m_graph.SetGraphType(graphPointType);

	int count = 0;
	double min = 0;
	double max = 0;
	double dy = 0;

	vector<vector<DOUBLE>> dataX(1);
	vector<vector<DOUBLE>> dataY(1);

	count = curStatistic.outNetwValuesValues[descNum][jointNum].size();
	m_graph.SetMinMaxX(0, Parameters::ONE_AGENT_LIFE_TIME, FALSE);
	m_graph.SetUnits("время, мс", "Выходной потенциал нейрона", FALSE);

	min = curStatistic.calcMin(curStatistic.outNetwValuesValues[descNum][jointNum]);
	max = curStatistic.calcMax(curStatistic.outNetwValuesValues[descNum][jointNum]);

	dataX[0].resize(count);
	dataY[0].resize(count);

	for(int i=0; i<count; i++) {
		dataX[0][i] = i*Parameters::ONE_AGENT_LIFE_TIME/60;
		dataY[0][i] = curStatistic.outNetwValuesValues[descNum][jointNum][i];
	}

	m_graph.SetMinMaxY(floor(min), ceil(max), FALSE);
	m_graph.SetCaliber(2, FALSE);
	m_graph.Update();

	m_graph.SetAllData(dataX, dataY);
}

void AgentUtil::writeTimeOfCalculation(time_t startTime, time_t endTime)
{
	LOG::startFileInput("log.txt");
	LOG::writeMessageInFile("time of calculation:");
	LOG::writeParamInFile(difftime(endTime,startTime));
	LOG::endFileInput();
}

void AgentUtil::setMode(int m)
{
	_mode = m;
}

int AgentUtil::getMode()
{
	return _mode;
}

bool AgentUtil::writeString(char*& from, string& inf, int& size)
{
	if(size + inf.size() <= Constants::SHARED_MEMORY_SIZE)
	{
		char* p = (char*)from;
		for(int i=0; i<inf.size(); ++i)
		{
			p[i] = inf.data()[i];
		}
		size += inf.size();
		from += inf.size();
		return true;
	}
	else
		return false;
}


bool AgentUtil::writeInt(char*& from, int& inf, int& size)
{
	if(size + Constants::INT_SIZE <= Constants::SHARED_MEMORY_SIZE)
	{
		int* p = (int*)from;
		p[0] = inf;
		size += Constants::INT_SIZE;
		from += Constants::INT_SIZE;
		return true;
	}
	else
		return false;
}

bool AgentUtil::writeDouble(char*& from, double& inf, int& size)
{
	if(size + Constants::DOUBLE_SIZE <= Constants::SHARED_MEMORY_SIZE)
	{
		double* p = (double*)from;
		p[0] = inf;
		size += Constants::DOUBLE_SIZE;
		from += Constants::DOUBLE_SIZE;
		return true;
	}
	else
		return false;
}

bool AgentUtil::writeBool(char*& from, bool& inf, int& size)
{
	if(size + Constants::BOOL_SIZE <= Constants::SHARED_MEMORY_SIZE)
	{
		bool* p = (bool*)from;
		p[0] = inf;
		size += Constants::BOOL_SIZE;
		from += Constants::BOOL_SIZE;
		return true;
	}
	else
		return false;
}

void AgentUtil::readInt(char*& from, int& inf)
{
	int* p = (int*)from;
	inf = p[0];
	from += Constants::INT_SIZE;
}

void AgentUtil::readDouble(char*& from,double& inf)
{
	double* p = (double*)from;
	inf = p[0];
	from += Constants::DOUBLE_SIZE;
}

void AgentUtil::readBool(char*& from,bool& inf)
{
	bool* p = (bool*)from;
	inf = p[0];
	from += Constants::BOOL_SIZE;
}
