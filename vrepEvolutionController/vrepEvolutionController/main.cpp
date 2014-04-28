#include "AgentUtil.h"
#include "GUIUtil.h"
#include <time.h>
#include "Statistics.h"

struct ThreadManager
{
	ThreadManager(string _name): name(_name){nValue = 0;}
	HWND parent;
	string name;
	int nValue;
};

static time_t startTime;
static time_t endTime;
static time_t genEndTime;
Generation generation;
HINSTANCE theInstance;
ThreadManager tm_A("evolution_thread");
HANDLE hThreadA;
extern ParamsController* paramsController;

DWORD WINAPI ThreadFuncA(LPVOID);

void mainCommand (int id, HWND hwnd, int command = 0)
{
	if(command == WM_CLOSE)//close app
	{
		generation.clearDescendants();
		if(AgentUtil::getMode() == Constants::CALCULATION_MODE)
		{
			AgentUtil::terminateAgentProcesses();
			AgentUtil::clear();
			AgentUtil::setMode(Constants::IDLE_MODE);
		}
	}
    switch (id)
    {
		case ID_CALCULATIONS_START://start process of evolution
		{
			if(AgentUtil::getMode() == Constants::IDLE_MODE)
			{
				if(!AgentUtil::agentsAreLoaded)
				{
					generation.clearDescendants();
					AgentUtil::curStatistic.clear();
				}

				AgentUtil::statsIsLoaded = false;
				paramsController->updateParametersFromGui();
				AgentUtil::setMode(Constants::CALCULATION_MODE);
				tm_A.parent = hwnd;
				hThreadA = CreateThread(NULL,0,ThreadFuncA,&tm_A,0,NULL);
			}
		}
        break;

		case ID_CALCULATIONS_STOP://start process of evolution
		{
			if(AgentUtil::getMode() == Constants::CALCULATION_MODE)
			{
				AgentUtil::terminateAgentProcesses();
				AgentUtil::clear();
				
				TerminateThread(hThreadA,0);
				AgentUtil::setMode(Constants::IDLE_MODE);
				AgentUtil::agentsAreLoaded = false;
			}
		}
        break;

		case ID_GRAPH_SAVE://start process of evolution
		{
			saveGraph();
		}
        break;

		case ID_NETWORKS_SAVE:
		{
			saveAgents(generation);
		}
		break;

		case ID_NETWORKS_LOAD:
		{
			if(AgentUtil::getMode() == Constants::IDLE_MODE)
			{
				generation.clearDescendants();
				AgentUtil::curStatistic.clear();

				loadAgents(generation);
				paramsController->setParametersToGUI();
				AgentUtil::agentsAreLoaded = true;
			}
		}
		break;

		case ID_NETWORKS_SAVEBESTAGENT:
		{
			saveBestAgent(generation);
		}
		break;

		case ID_NETWORKS_LOADBESTAGENT:
		{
			if(AgentUtil::getMode() == Constants::IDLE_MODE)
			{
				generation.clearDescendants();
				AgentUtil::curStatistic.clear();

				loadBestAgent(generation);
				paramsController->setParametersToGUI();
				AgentUtil::agentsAreLoaded = true;
			}
		}
		break;

		case ID_PARAMETERS_LOADFR:
		{
			if(AgentUtil::getMode() == Constants::IDLE_MODE)
			{
				loadParams();
				paramsController->setParametersToGUI();
			}
		}
		break;

		case ID_PARAMETERS_SAVEASDEFAULT:
		{
			paramsController->updateParametersFromGui();
			AgentUtil::saveDefaultParams();
		}
		break;

		case ID_PARAMETERS_SAVETOFILE:
		{
			paramsController->updateParametersFromGui();
			saveParams();
		}
		break;
		case ID_RESULTS_SAVESTATISTICS:
		{
			saveResults();
		}
		break;
		case ID_RESULTS_SAVEAVERAGESTATISTIC:
		{
			saveAvResults();
		}
		break;
		case ID_RESULTS_LOADSTATISTICS:
		{
			if(AgentUtil::getMode() == Constants::IDLE_MODE)
			{
				AgentUtil::clearStatBuffer =  paramsController->clearBuffer->isChecked();
				loadResults();
				AgentUtil::statsIsLoaded = true;
				AgentUtil::cur_graph_type = Constants::STATISTIC_TYPE_GRAPH;
				AgentUtil::cur_graph_selection = paramsController->selectResults->getCurSelection();
				AgentUtil::plotGraph(m_Graph, paramsController->typeOfGraph->getCurSelection(), paramsController->graphDescNum->getInt());
			}
		}
		break;
		case ID_RESULTS_SHOWBESTAGENT:
		{
			if(AgentUtil::getMode() == Constants::IDLE_MODE || AgentUtil::getMode() == Constants::SHOW_MODE)
			{
				AgentUtil::setMode(Constants::SHOW_MODE);
				AgentUtil::showBestAgent(generation,hwnd);
			}
		}
		break;
		case ID_RESULTS_SHOWNEXTAGENT:
		{
			if(AgentUtil::getMode()  == Constants::IDLE_MODE || AgentUtil::getMode() == Constants::SHOW_MODE)
			{
				AgentUtil::setMode(Constants::SHOW_MODE);
				AgentUtil::showNextAgent(generation,hwnd);
			}
		}
		break;
		case ID_RESULTS_SHOWPREVAGENT:
		{
			if(AgentUtil::getMode() == Constants::IDLE_MODE || AgentUtil::getMode() == Constants::SHOW_MODE)
			{
				AgentUtil::setMode(Constants::SHOW_MODE);
				AgentUtil::showPrevAgent(generation,hwnd);
			}
		}
		break;
		case ID_RESULTS_STOPSHOWING:
		{
			if(AgentUtil::getMode() == Constants::SHOW_MODE)
			{
				AgentUtil::terminateShowAgentProcess();
				AgentUtil::clearAfterShowAgent();
				AgentUtil::setMode(Constants::IDLE_MODE);
			}
		}
		break;

		case IDC_COMBO_SELECT_RESULTS:
		if(command == CBN_SELCHANGE)
		{
			AgentUtil::cur_graph_type = Constants::STATISTIC_TYPE_GRAPH;
			AgentUtil::cur_graph_selection = paramsController->selectResults->getCurSelection();
			AgentUtil::plotGraph(m_Graph, paramsController->typeOfGraph->getCurSelection(), paramsController->graphDescNum->getInt());
		}
        break;

		case IDC_COMBO_TYPE_OF_GRAPH:
		if(command == CBN_SELCHANGE)
		{
			AgentUtil::plotGraph(m_Graph, paramsController->typeOfGraph->getCurSelection(), paramsController->graphDescNum->getInt());
		}
		break;

		case IDC_COMBO_GRAPH_JOINT:
		{
			if(command == CBN_SELCHANGE)
			{
				AgentUtil::cur_graph_type = Constants::JOINT_TYPE_GRAPH;
				AgentUtil::cur_graph_selection = paramsController->typeOfJoint->getCurSelection();
				AgentUtil::plotGraph(m_Graph, paramsController->typeOfGraph->getCurSelection(), paramsController->graphDescNum->getInt());
			}
		}
		break;

		case IDC_COMBO_GRAPH_OUTP_NEURON:
		{
			if(command == CBN_SELCHANGE)
			{
				AgentUtil::cur_graph_type = Constants::NETWORK_TYPE_GRAPH;
				AgentUtil::cur_graph_selection = paramsController->outputNeuron->getCurSelection();
				AgentUtil::plotGraph(m_Graph, paramsController->typeOfGraph->getCurSelection(), paramsController->graphDescNum->getInt());
			}
		}

		case IDC_BUTTON_RENAME_LEGEND:
		{
			if(command == BN_CLICKED)
			{
				createRenameLegendDialog();
			}
		}
        break;

		case IDC_BUTTON_SAVE_FROM_NNETWORK:
		{
			if(command == BN_CLICKED)
			{
				AgentUtil::saveFromNetwork(generation, paramsController->saveFromNetworks->getCurSelection());
			}
		}
		break;

		case IDC_BUTTON_SAVE_FOR_BEST_AGENT:
		{
			if(command == BN_CLICKED)
			{
				AgentUtil::saveFromNetwork(generation, paramsController->saveFromNetworks->getCurSelection(), true);
			}
		}
		break;

		case IDC_BUTTON_SAVE_FROM_STATISTIC:
		{
			if(command == BN_CLICKED)
			{
				AgentUtil::saveFromStatistic(paramsController->saveFromStatistic->getCurSelection());
			}
		}
		break;
	}
}

//-----------------------------------------------
DWORD WINAPI ThreadFuncA(LPVOID lpv)
{
	AgentUtil::initialize();
	
	AgentUtil::createAgentProcesses(tm_A.parent);

	while(AgentUtil::cur_lunch < Parameters::LAUNCH_NUM)
	{
		srand(Parameters::SEED);
		AgentUtil::resetLaunch();
		generation.reset();
		AgentUtil::curStatistic.clear();

		if(Parameters::autoloadParams)
		{
			AgentUtil::loadParamsFromFile("params_"+Util::to_string(AgentUtil::cur_lunch)+".prm");
			paramsController->setParametersToGUI();
		}

		Parameters::printParams();

		if(Parameters::autosaveParams)
			AgentUtil::saveParamsToFile("savedParams_"+Util::to_string(AgentUtil::cur_lunch)+".prm");

		//main cycle of evolution
		startTime = time(NULL);
		bool sucsessWr = true;
		while(generation.hasNextGenertion() && sucsessWr)
		{
			AgentUtil::resetBeforeNewGeneration();
			generation.createNewGeneration(!AgentUtil::agentsAreLoaded);
			sucsessWr = AgentUtil::writeNetworksInFiles(generation);
			if(sucsessWr)
			{
				AgentUtil::sendMessagesToAgents(Constants::INITIALIZE_CYCLE_MESSAGE);
				AgentUtil::waitForMessagesFromAgents(Constants::AGENT_IS_READY_MESSAGE);
				AgentUtil::sendMessagesToAgents(Constants::START_LIFE_MESSAGE);
				AgentUtil::waitForMessagesFromAgents(Constants::AGENT_LIFE_ENDED_MESSAGE);
				AgentUtil::readAgentLifeInfo(generation);
				AgentUtil::curStatistic.saveResults(generation);
				AgentUtil::plotGraph(m_Graph, paramsController->typeOfGraph->getCurSelection(), paramsController->graphDescNum->getInt());
				
				if((generation.curGeneration+1)%Parameters::SAVE_GENERATION_NUM == 0)
					AgentUtil::saveRuntimeInfo(generation);

				genEndTime = time(NULL);
				paramsController->console->addMessageS("Generation: " + Util::to_string(generation.curGeneration));
				paramsController->console->addMessageS("time: " + Util::to_string(difftime(genEndTime,startTime)));
				generation.curGeneration++;
			}
		}

		if(sucsessWr)
		{
			if(generation.generationNum%Parameters::SAVE_GENERATION_NUM != 0)//write the end generation
				AgentUtil::saveRuntimeInfo(generation);
		}
		else
		{
			LOG::startFileInput();
			LOG::writeMessageInFile("Calculation end with error: Shared memory ended");
			LOG::endFileInput();
		}

		endTime = time(NULL);
		AgentUtil::writeTimeOfCalculation(startTime, endTime);

		++AgentUtil::cur_lunch;
	}

	AgentUtil::sendMessagesToAgents(Constants::EXIT_PROCESS_MESSAGE);
	AgentUtil::terminateAgentProcesses();
	AgentUtil::clear();
	AgentUtil::agentsAreLoaded = false;
	AgentUtil::setMode(Constants::IDLE_MODE);

	return 0;
}
//-----------------------------------------------

int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hPrevInst, char * cmdParam, int cmdShow)
{
	theInstance = hInst;
	
	CoCreateGuid(&AgentUtil::guid);
	int msgWParam = realizeGUI(hInst);

	
   
    return msgWParam;
}