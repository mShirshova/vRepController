#include "GUIUtil.h"
#include "Parameters.h"
#include <Commctrl.h>
#include "LOG.h"

#define ARRAY_SIZE(_) (sizeof (_) / sizeof (*_))

extern HINSTANCE theInstance;

ParamsController* paramsController = NULL;
AzGraphCtrl m_Graph;

HWND mainDialogHWND;

const int TAB_NUM = 6;
HWND hTab;
HWND g_hDialogs [TAB_NUM] = { 0 };
INT g_nDialogs [TAB_NUM] = {IDD_PAGE_COMMON, IDD_PAGE_NETWORK, IDD_PAGE_EVOLUTION, IDD_PAGE_GRAPH, IDD_PAGE_CONCOLE, IDD_DIALOG_SAVE_TO_FILE};

COLORREF WHITE_CL = RGB(255,255,255);
COLORREF GREY_CL = RGB(70,70,70);
HBRUSH WHITE_BR;
HBRUSH GREY_BR;

void saveSomeFile(char (&fileName)[MAX_PATH], char* lpstrFilter, char* lpstrTitle)
{
    OPENFILENAME ofn;
    
    *fileName=0;

    memset(&ofn,0,sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = sizeof(fileName);
    ofn.lpstrFilter = lpstrFilter;//"Тип 1\0*.exe\0Тип 2\0*.dll\0Тип MS\0ms*.*";
    ofn.nFilterIndex = 1;//
    ofn.lpstrTitle = lpstrTitle;
    ofn.lpstrInitialDir = "c:\\windows";
    ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;

	GetSaveFileName(&ofn);
}

void saveGraph()
{
	char fileName[MAX_PATH];
	saveSomeFile(fileName, "bmp\0*.bmp", "Сохранение графика");
	string extName(fileName);
	extName += ".bmp";
	HBITMAP b = m_Graph.GetVisualBitmap();
	if(*fileName != 0)
		m_Graph.SaveToFile(b,extName.data());
}

void saveAgents(Generation& g)
{
	char fileName[MAX_PATH];
	saveSomeFile(fileName, "agnt\0*.agnt", "Сохранение всех агентов");
	string extName(fileName);
	extName += ".agnt";

	if(*fileName != 0)
		AgentUtil::saveAgentsToFile(g, extName);
}

void loadAgents(Generation& g)
{
	char fileName[MAX_PATH];
	saveSomeFile(fileName, "agnt\0*.agnt", "Загрузка всех агентов");
	string extName(fileName);

	if(*fileName != 0)
		AgentUtil::loadAgentsFromFile(g, fileName);
}

void saveBestAgent(Generation& g)
{
	char fileName[MAX_PATH];
	saveSomeFile(fileName, "agnt\0*.agnt", "Сохранение лучшего агента");
	string extName(fileName);
	extName += ".agnt";

	if(*fileName != 0)
		AgentUtil::saveBestAgentToFile(g, extName);
}

void loadBestAgent(Generation& g)
{
	char fileName[MAX_PATH];
	saveSomeFile(fileName, "agnt\0*.agnt", "Загрузка лучшего агента");
	string extName(fileName);

	if(*fileName != 0)
		AgentUtil::loadBestAgentFromFile(g, fileName);
}

void saveParams()
{
	char fileName[MAX_PATH];
	saveSomeFile(fileName, "prm\0*.prm", "Сохранение параметров");
	string extName(fileName);
	extName += ".prm";

	if(*fileName != 0)
		AgentUtil::saveParamsToFile(extName);
}

void loadParams()
{
	char fileName[MAX_PATH];
	saveSomeFile(fileName, "prm\0*.prm", "Загрузка параметров");
	string extName(fileName);

	if(*fileName != 0)
		AgentUtil::loadParamsFromFile(fileName);
}

void saveResults()
{
	char fileName[MAX_PATH];
	saveSomeFile(fileName, "rslt\0*.rslt", "Сохранение результатов");
	string extName(fileName);
	extName += ".rslt";

	if(*fileName != 0)
		AgentUtil::saveResults(extName, Parameters::programVersion);
}

void saveAvResults()
{
	char fileName[MAX_PATH];
	saveSomeFile(fileName, "rslt\0*.rslt", "Сохранение средних результатов");
	string extName(fileName);
	extName += ".rslt";

	if(*fileName != 0)
		AgentUtil::saveAvStatistic(extName);
}

void loadResults()
{
	char fileName[MAX_PATH];
	saveSomeFile(fileName, "rslt\0*.rslt", "Загрузка результатов");
	string extName(fileName);

	if(*fileName != 0)
		AgentUtil::loadResults(fileName);
}

void ParamsController::initializeControls(int pageID, HWND hwnd)
{
	if(pageID == IDD_PAGE_COMMON)
	{
		generationNum = new Edit(hwnd, IDC_EDIT_GEN_NUM);
		descendantNum = new Edit(hwnd, IDC_EDIT_DESC_NUM);
		agentLifeTime = new Edit(hwnd, IDC_EDIT_AGENT_LIFE_TIME);
		
		fitnessType = new MComboBox(hwnd, IDC_COMBO_FITNESS_TYPE);
		controlType = new MComboBox(hwnd, IDC_COMBO_TYPE_OF_CONTROL);

		angleStepSize = new Edit(hwnd, IDC_EDIT_ANGLE_STEP_SIZE);
		dimOfInput = new MComboBox (hwnd, IDC_COMBO_DIM_OF_INPUT);
		robotNumInOneSession = new Edit(hwnd, IDC_EDIT_ROB_NUM_IN_ONE_SESSION);
		saveGenerationNum = new Edit(hwnd, IDC_EDIT_SAVE_GEN_NUM);
		seed = new Edit(hwnd, IDC_EDIT_SEED);
		naoNumInScene = new Edit(hwnd, IDC_EDIT_NAO_NUM_IN_SCENE);

		launchNum = new Edit(hwnd, IDC_EDIT_LAUNCH_NUM);
		autoLoadParams = new CheckBox(hwnd, IDC_CHECK_AUTOLOAD_PARAMS);
		autoSaveParams = new CheckBox(hwnd, IDC_CHECK_AUTOSAVE_PARAMS);
		autoSaveNetwroks = new CheckBox(hwnd, IDC_CHECK_AUTOSAVE_NETWROKS);
		autoSaveStatistic = new CheckBox(hwnd, IDC_CHECK_AUTOSAVE_STATISTIC);
		showJointData = new CheckBox(hwnd, IDC_CHECK_SHOW_JOINT_DATA);

		_initilizedPageCount++;
	}
	else if(pageID == IDD_PAGE_NETWORK)
	{
		neuronType = new MComboBox(hwnd, IDC_COMBO_NEURON_TYPE);
		inputTransFuncType = new MComboBox(hwnd, IDC_COMBO_INPUT_TRANS_FUNC_TYPE);
		intermTransFuncType = new MComboBox(hwnd, IDC_COMBO_INTERM_TRANS_FUNC_TYPE);
		outTransFuncType = new MComboBox(hwnd, IDC_COMBO_OUT_TRANS_FUNC_TYPE);
		logLinearMax = new Edit(hwnd, IDC_EDIT_LOG_LINEAR_MAX);
		logFuncKCoeff = new Edit(hwnd, IDC_EDIT_LOG_FUNC_COEFF);
		useNormalization = new CheckBox(hwnd, IDC_CHECK_USE_NORMAL);
		useRandomAddition = new CheckBox(hwnd, IDC_CHECK_USE_RAND_ADD_2);
		maxRandWeight = new Edit(hwnd, IDC_EDIT_MAX_RAND_WEIGHT);
		maxPotential = new Edit(hwnd, IDC_EDIT_MAX_POTENTIAL);
		minPotential = new Edit(hwnd, IDC_EDIT_MIN_POTENTIAL);
		activationThr = new Edit(hwnd, IDC_EDIT_ACT_THRESHOLD);
		gauseBoundValue = new Edit(hwnd, IDC_EDIT_GAUSE_BOUND_VALUE);
		uRand = new Edit(hwnd, IDC_EDIT_U_RAND_ADD);
		aRand = new Edit(hwnd, IDC_EDIT_A_RAND);
		resultThreshold = new Edit(hwnd, IDC_EDIT_RESULT_THRESHOLD);
		delayTime = new Edit(hwnd, IDC_EDIT_DELAY_TIME);
		outputMultiplyCoeff = new Edit(hwnd, IDC_EDIT_OUTPUT_MULTIPLY);
		sinFrequency = new Edit(hwnd, IDC_EDIT_SIN_FREQUENCY);
		startIntermNeurons = new Edit(hwnd, IDC_EDIT_START_INTERM_NEURONS);

		addInhibitoryConnectionToOtput = new CheckBox(hwnd, IDC_CHECK_ADD_INHIBITORY); 
		addBackConnectionToOtput = new CheckBox(hwnd, IDC_CHECK_ADD_BACK_TO_OUTPUT);

		useReadynetwork = new CheckBox(hwnd, IDC_CHECK_USE_READY_NETWORK_2);
		readyNetworkType = new MComboBox(hwnd, IDC_COMBO_NETWORK_TYPE);
		neuronsMatrix = new Edit(hwnd, IDC_EDIT_NEURON_MATRIX);
		fullyNetNeuronNum = new Edit(hwnd, IDC_EDIT_FULLY_NET_NEURON_NUM);
		inputDelayNeuronNum = new Edit(hwnd, IDC_EDIT_INPUT_DELAY_NUM);
		backFromOutputDelayNum =new Edit(hwnd, IDC_EDIT_BACK_FROM_OUTPUT_DELAY_NUM);
		_initilizedPageCount++;
	}
	else if(pageID == IDD_PAGE_EVOLUTION)
	{
		evAlgType = new MComboBox(hwnd, IDC_COMBO_EV_ALG_TYPE);
		descCreationType = new MComboBox(hwnd, IDC_COMBO_DESC_CREAT_TYPE);
		selectionType = new MComboBox(hwnd, IDC_COMBO_SELECTION_TYPE);
		
		portionsOfChilds = new Edit(hwnd, IDC_EDIT_P_CROSSOVER);

		selectFromStable = new CheckBox(hwnd, IDC_CHECK_SELECT_FROM_STABLE);
		mutateStablePart = new CheckBox(hwnd, IDC_CHECK_MUTATE_STABLE);

		pWeightMut= new Edit(hwnd, IDC_EDIT_P_WEIGHT_MUT);
		pThresholdMut = new Edit(hwnd, IDC_EDIT_P_THRESH_MUT);
		dispersWeight = new Edit(hwnd, IDC_EDIT_WEIGHT_DISPERSION);
		dispersThreshold = new Edit(hwnd, IDC_EDIT_THRESH_DISPERSION);
		pDublication = new Edit(hwnd, IDC_EDIT_P_DUBLICATION);
		pDeleteion = new Edit(hwnd, IDC_EDIT_P_DELETION);
		pConnectionAdd = new Edit(hwnd, IDC_EDIT_P_CONNECTION_ADD);
		pConnectionRemove = new Edit(hwnd, IDC_EDIT_P_CONNECTION_REMOVE);
		pMutTime = new Edit(hwnd, IDC_EDIT_P_MUT_TIME);
		pMutResThr = new Edit(hwnd, IDC_EDIT_P_MUT_RES_THR);
		dispersTime = new Edit(hwnd, IDC_EDIT_TIME_DISPERS);
		dispersResThr = new Edit(hwnd, IDC_EDIT_RES_THR_DISPERS);
		pMutFrequency = new Edit(hwnd, IDC_EDIT_P_MUT_FREQUENCY);
		dispersFrequency = new Edit(hwnd, IDC_EDIT_DISPERS_FREQUENCY);

		dublicateInputNeurons = new CheckBox(hwnd, IDC_CHECK_DUBLICATE_INPUT_NEURONS);
		dublicateOutputNeurons = new CheckBox(hwnd, IDC_CHECK_DUBLICATE_OUTPUT_NEURONS);
		deleteInputNeurons = new CheckBox(hwnd, IDC_CHECK_DELETE_INPUT_NEURONS);
		deleteOutputNeurons = new CheckBox(hwnd, IDC_CHECK_DELETE_OUTPUT_NEURONS);

		_initilizedPageCount++;
	}
	else if(pageID == IDD_PAGE_GRAPH)
	{
		selectResults = new MComboBox(hwnd, IDC_COMBO_SELECT_RESULTS);
		typeOfGraph = new MComboBox(hwnd, IDC_COMBO_TYPE_OF_GRAPH);
		graphDescNum = new Edit(hwnd, IDC_EDIT_GRAFH_DESCENDANT);
		typeOfJoint = new MComboBox(hwnd, IDC_COMBO_GRAPH_JOINT);
		outputNeuron = new MComboBox(hwnd, IDC_COMBO_GRAPH_OUTP_NEURON);
		clearBuffer = new CheckBox(hwnd, IDC_CHECK_CLEAR_BUFFER);
		renameLegend = new Button(hwnd, IDC_BUTTON_RENAME_LEGEND);
		_initilizedPageCount++;
	}
	else if(pageID == IDD_PAGE_CONCOLE)
	{
		_initilizedPageCount++;
		console = new Edit(hwnd, IDC_EDIT1);
	}
	else if(pageID == IDD_DIALOG_SAVE_TO_FILE){
		_initilizedPageCount++;
		saveFromStatistic = new MComboBox(hwnd, IDC_COMBO_SAVE_FROM_STATISTIC);
		saveFromNetworks = new MComboBox(hwnd, IDC_COMBO_SAVE_FROM_NETWORK);
		saveFromStatisticButton = new Button(hwnd, IDC_BUTTON_SAVE_FROM_STATISTIC);
		saveFromNetworksButton = new Button(hwnd, IDC_BUTTON_SAVE_FROM_NNETWORK);
		saveForBestAgent = new Button(hwnd, IDC_BUTTON_SAVE_FOR_BEST_AGENT);
	}
}

 ParamsController::~ParamsController ()
 {
	delete generationNum;
	delete descendantNum;
	delete agentLifeTime;
		
	delete fitnessType;
	delete controlType;

	delete angleStepSize;
	delete dimOfInput;
	delete robotNumInOneSession;
	delete saveGenerationNum;
	delete seed;
	delete naoNumInScene;

	delete autoLoadParams;
	delete launchNum;
	delete autoSaveNetwroks;
	delete autoSaveParams;
	delete autoSaveStatistic;
	delete showJointData;

	delete neuronType;
	delete inputTransFuncType;
	delete intermTransFuncType;
	delete outTransFuncType;
	delete logLinearMax;
	delete logFuncKCoeff;
	delete useNormalization;
	delete useRandomAddition;
	delete maxRandWeight;
	delete maxPotential;
	delete minPotential;
	delete activationThr;
	delete gauseBoundValue;
	delete uRand;
	delete aRand;
	delete resultThreshold;
	delete delayTime;
	delete outputMultiplyCoeff;
	delete sinFrequency;
	delete startIntermNeurons;

	delete addInhibitoryConnectionToOtput;
	delete addBackConnectionToOtput;

	delete useReadynetwork;
	delete readyNetworkType;
	delete neuronsMatrix;
	delete fullyNetNeuronNum;
	delete inputDelayNeuronNum;
	delete backFromOutputDelayNum;

	delete evAlgType;
	delete descCreationType;
	delete selectionType;
	delete portionsOfChilds;
	delete selectFromStable;
	delete mutateStablePart;

	delete pWeightMut;
	delete pThresholdMut;
	delete dispersWeight;
	delete dispersThreshold;
	delete pDublication;
	delete pDeleteion;
	delete pConnectionAdd;
	delete pConnectionRemove;
	delete pMutTime;
	delete pMutResThr;
	delete dispersTime;
	delete dispersResThr;
	delete pMutFrequency;
	delete dispersFrequency;

	delete dublicateInputNeurons;
	delete dublicateOutputNeurons;
	delete deleteInputNeurons;
	delete deleteOutputNeurons;

	delete console;
	delete selectResults;
	delete typeOfGraph;
	delete graphDescNum;
	delete typeOfJoint;
	delete outputNeuron;
	delete clearBuffer;
	delete renameLegend;

	delete saveFromStatistic;
	delete saveFromNetworks;
	delete saveFromNetworksButton;
	delete saveFromStatisticButton;
	delete saveForBestAgent;
 }

bool ParamsController::isAllInitialized()
{
	return _initilizedPageCount == TAB_NUM;
}

void ParamsController::setParametersToGUI()
{
	char buff[50];
	sprintf_s(buff,"%d",Parameters::GENERATION_NUM);
	generationNum->setString(buff);
	sprintf_s(buff,"%d",Parameters::DESCENDANT_NUM);
	descendantNum->setString(buff);
	sprintf_s(buff,"%d",Parameters::ONE_AGENT_LIFE_TIME);
	agentLifeTime->setString(buff);

	vector<string> temp;
	
	temp.resize(3);
	temp[0] = "DISTANCE";
	temp[1] = "ACTIVITY";
	temp[2] = "MIX";
	//temp[3] = "HEIGHT_DISTANCE";
	fitnessType->clear();
	fitnessType->setItems(temp);
	fitnessType->setCurSelection(Parameters::typeOfFitness);

	temp[0] = "COMPETITION";
	temp[1] = "DELTA_COMPETTION";
	temp[2] = "PROP_COMPETTION";
	temp.push_back("ONE_NEURON_CONTROL");
	temp.push_back("MULTIPLE_COMPETTITION_CONTROL");
	temp.push_back("FORCE_CONTROL");
	controlType->clear();
	controlType->setItems(temp);
	controlType->setCurSelection(Parameters::typeOfControl);

	sprintf_s(buff,"%f",Parameters::ANGLE_STEP_SIZE);
	angleStepSize->setString(buff);

	temp.resize(2);
	temp[0] = "FULL_DIM";
	temp[1] = "MIDDLE_DIM";
	dimOfInput->clear();
	dimOfInput->setItems(temp);
	dimOfInput->setCurSelection(Parameters::dimentionOfInput);

	sprintf_s(buff,"%d",Parameters::PROCCESSES_NUM);
	robotNumInOneSession->setString(buff);

	sprintf_s(buff,"%d",Parameters::SAVE_GENERATION_NUM);
	saveGenerationNum->setString(buff);

	sprintf_s(buff,"%d",Parameters::SEED);
	seed->setString(buff);

	sprintf_s(buff,"%d",Parameters::NAO_NUM_IN_SCENE);
	naoNumInScene->setString(buff);

	sprintf_s(buff,"%d",Parameters::LAUNCH_NUM);
	launchNum->setString(buff);

	if(Parameters::autoloadParams)
		autoLoadParams->check();
	else
		autoLoadParams->unCheck();

	if(Parameters::autosaveParams)
		autoSaveParams->check();
	else
		autoSaveParams->unCheck();

	if(Parameters::autosaveNetworks)
		autoSaveNetwroks->check();
	else
		autoSaveNetwroks->unCheck();

	if(Parameters::autosaveStatistic)
		autoSaveStatistic->check();
	else
		autoSaveStatistic->unCheck();

	if(Parameters::showJointsData)
		showJointData->check();
	else
		showJointData->unCheck();

	temp.resize(4);
	temp[0] = "SIMPLE_NEURON";
	temp[1] = "TIME_DELAY_NEURON";
	temp[2] = "AR_NEURON";
	temp[3] = "LEAKY_INTEGRATOR_NEURONS";
	neuronType->clear();
	neuronType->setItems(temp);
	neuronType->setCurSelection(Parameters::neuronType);

	temp.resize(6);
	temp[0] = "LOGISTIC_FUNCTION";
	temp[1] = "THRESHOLD_FUNCTION";
	temp[2] = "GAUSS_FUNCTION";
	temp[3] = "LOG_AND_THRESHOLD_FUNCTION";
	temp[4] = "SIN_FUNCTION";
	temp[5] = "NONE_FUNCTION";
	inputTransFuncType->clear();
	intermTransFuncType->clear();
	outTransFuncType->clear();
	inputTransFuncType->setItems(temp);
	inputTransFuncType->setCurSelection(Parameters::inputTransmissionFuncType);
	intermTransFuncType->setItems(temp);
	intermTransFuncType->setCurSelection(Parameters::intermTransmissionFuncType);
	outTransFuncType->setItems(temp);
	outTransFuncType->setCurSelection(Parameters::outTransmissionFuncType);

	sprintf_s(buff,"%f",Parameters::LOG_FUNC_LINEAR_MAX);
	logLinearMax->setString(buff);

	sprintf_s(buff,"%f",Parameters::LOG_FUNC_K_COEFF);
	logFuncKCoeff->setString(buff);

	if(Parameters::useNormalization)
		useNormalization->check();
	else
		useNormalization->unCheck();

	if(Parameters::useRandomAddition)
		useRandomAddition->check();
	else
		useRandomAddition->unCheck();

	sprintf_s(buff,"%f",Parameters::MAX_POTENTIAL);
	maxPotential->setString(buff);

	sprintf_s(buff,"%f",Parameters::MIN_POTENTIAL);
	minPotential->setString(buff);

	sprintf_s(buff,"%f",Parameters::ACTIVATION_THR);
	activationThr->setString(buff);

	sprintf_s(buff,"%f",Parameters::GAUSSE_BOUND_VALUE);
	gauseBoundValue->setString(buff);

	sprintf_s(buff,"%f",Parameters::U_RAND);
	uRand->setString(buff);

	sprintf_s(buff,"%f",Parameters::A_RAND);
	aRand->setString(buff);

	sprintf_s(buff,"%f",Parameters::MAX_RANDOM_WEIGHT);
	maxRandWeight->setString(buff);

	sprintf_s(buff,"%f",Parameters::RESULT_THR);
	resultThreshold->setString(buff);

	sprintf_s(buff,"%d",Parameters::NEURON_DELAY_TIME);
	delayTime->setString(buff);

	sprintf_s(buff,"%f",Parameters::OUT_MILTIPLY_COEFF);
	outputMultiplyCoeff->setString(buff);

	sprintf_s(buff,"%f",Parameters::SIN_FREQUENCY);
	sinFrequency->setString(buff);

	sprintf_s(buff,"%d",Parameters::START_INTERM_NEURON_NUM);
	startIntermNeurons->setString(buff);

	if(Parameters::addInhibitoryConnectionsToOutput)
		addInhibitoryConnectionToOtput->check();
	else 
		addInhibitoryConnectionToOtput->unCheck();

	if(Parameters::addBackConnectionToOutput)
		addBackConnectionToOtput->check();
	else 
		addBackConnectionToOtput->unCheck();

	if(Parameters::useReadyNetwork)
		useReadynetwork->check();
	else
		useReadynetwork->unCheck();

	temp.resize(3);
	temp[0] = "FEEDFORWARD_NETWORK_TYPE";
	temp[1] = "FULLY_CONNECTED_NETWORK";
	temp[2] = "JORDAN_SIMILAR_NETWORK";
	readyNetworkType->clear();
	readyNetworkType->setItems(temp);
	readyNetworkType->setCurSelection(Parameters::networkType);

	neuronsMatrix->setString((char*)Parameters::neuronsMatrix.data());

	sprintf_s(buff,"%d",Parameters::FULLY_NETW_NEURON_NUM);
	fullyNetNeuronNum->setString(buff);

	sprintf_s(buff,"%d",Parameters::INPUT_DELAY_NUM);
	inputDelayNeuronNum->setString(buff);

	sprintf_s(buff,"%d",Parameters::BACK_FROM_OUTPUT_DELAY_NUM);
	backFromOutputDelayNum->setString(buff);

	temp.resize(1);
	temp[0] = "GENETIC_ALG";
	evAlgType->clear();
	evAlgType->setItems(temp);
	evAlgType->setCurSelection(0);

	temp.resize(2);
	temp[0] = "ROULETTE_SEL_TYPE";
	temp[1] = "RANDOM_SEL_TYPE";
	selectionType->clear();
	selectionType->setItems(temp);
	selectionType->setCurSelection(Parameters::selectionType);

	temp.resize(3);
	temp[0] = "CREATE_DESC_WITH_CROSSOVER";
	temp[1] = "CREATE_DESC_MB";
	temp[2] = "CREATE_DESC_MB_2";
	descCreationType->clear();
	descCreationType->setItems(temp);
	descCreationType->setCurSelection(Parameters::descendantCreationType);

	sprintf_s(buff,"%f",Parameters::PORTION_OF_CHILDS);
	portionsOfChilds->setString(buff);

	if(Parameters::selectFromStable)
		selectFromStable->check();
	else
		selectFromStable->unCheck();

	if(Parameters::mutateStablePartOfPopulation)
		mutateStablePart->check();
	else
		mutateStablePart->unCheck();

	sprintf_s(buff,"%f",Parameters::P_MUTATION_WEIGHT);
	pWeightMut->setString(buff);

	sprintf_s(buff,"%f",Parameters::P_MUTATION_THRESHOLD);
	pThresholdMut->setString(buff);

	sprintf_s(buff,"%f",Parameters::S_WEGHT);
	dispersWeight->setString(buff);

	sprintf_s(buff,"%f",Parameters::S_THRESH);
	dispersThreshold->setString(buff);

	sprintf_s(buff,"%f",Parameters::P_DUPLICATION);
	pDublication->setString(buff);

	sprintf_s(buff,"%f",Parameters::P_DELETION);
	pDeleteion->setString(buff);

	sprintf_s(buff,"%f",Parameters::P_CONNECTION_ADD);
	pConnectionAdd->setString(buff);

	sprintf_s(buff,"%f",Parameters::P_CONNECTION_REMOVE);
	pConnectionRemove->setString(buff);

	sprintf_s(buff,"%f",Parameters::P_MUTATION_TIME);
	pMutTime->setString(buff);

	sprintf_s(buff,"%f",Parameters::P_MUTATION_RES_THRESHOLD);
	pMutResThr->setString(buff);

	sprintf_s(buff,"%f",Parameters::S_TIME_DELAY);
	dispersTime->setString(buff);

	sprintf_s(buff,"%f",Parameters::S_RES_THRESH);
	dispersResThr->setString(buff);

	sprintf_s(buff,"%f",Parameters::P_MUTATION_FREQUENCY);
	pMutFrequency->setString(buff);

	sprintf_s(buff,"%f",Parameters::S_FREQUENCY);
	dispersFrequency->setString(buff);

	if(Parameters::dublicateInputNeurons)
		dublicateInputNeurons->check();
	else
		dublicateInputNeurons->unCheck();

	if(Parameters::dublicateOutputNeurons)
		dublicateOutputNeurons->check();
	else
		dublicateOutputNeurons->unCheck();

	if(Parameters::deleteInputNeurons)
		deleteInputNeurons->check();
	else
		deleteInputNeurons->unCheck();

	if(Parameters::deleteOutputNeurons)
		deleteOutputNeurons->check();
	else
		deleteOutputNeurons->unCheck();

	temp.resize(16);
	temp[0] = "Max fintess";
	temp[1] = "Average fitness";
	temp[2] = "Max num of neurons";
	temp[3] = "Min mun of neurons";
	temp[4] = "Average num of neurons";
	temp[5] = "Max num of connections";
	temp[6] = "Min mun of connections";
	temp[7] = "Average mun of connections";
	temp[8] = "Best agent network consistency";
	temp[9] = "Average network consistency";
	temp[10] = "Best agent achieving result time";
	temp[11] = "Average network achieving result time";
	temp[12] = "Best agent av delay time";
	temp[13] = "Average av. delay time";
	temp[14] = "Dispers of best agent av delay time";
	temp[15] = "Average dispers of av delay time";

	selectResults->clear();
	selectResults->setItems(temp);
	selectResults->setCurSelection(0);

	temp.resize(2);
	temp[0] = "Point graph";
	temp[1] = "Line Graph";
	typeOfGraph->clear();
	typeOfGraph->setItems(temp);
	typeOfGraph->setCurSelection(0);

	graphDescNum->setString("0");

	temp.resize(18);
	temp[0] = "LEFT_ARM_JOINT_0";
	temp[1] = "LEFT_ARM_JOINT_1";
	temp[2] = "LEFT_ARM_JOINT_2";
	temp[3] = "RIGHT_ARM_JOINT_0";
	temp[4] = "RIGHT_ARM_JOINT_1";
	temp[5] = "RIGHT_ARM_JOINT_2";
	temp[6] = "RIGHT_LEG_JOINT_0";
	temp[7] = "RIGHT_LEG_JOINT_1";
	temp[8] = "RIGHT_LEG_JOINT_2";
	temp[9] = "RIGHT_LEG_JOINT_3";
	temp[10] = "RIGHT_LEG_JOINT_4";
	temp[11] = "RIGHT_LEG_JOINT_5";
	temp[12] = "LEFT_LEG_JOINT_0";
	temp[13] = "LEFT_LEG_JOINT_1";
	temp[14] = "LEFT_LEG_JOINT_2";
	temp[15] = "LEFT_LEG_JOINT_3";
	temp[16] = "LEFT_LEG_JOINT_4";
	temp[17] = "LEFT_LEG_JOINT_5";

	typeOfJoint->clear();
	typeOfJoint->setItems(temp);
	typeOfJoint->setCurSelection(0);

	temp.resize(Parameters::OUTPUT_NUM);
	for(int i=0; i<Parameters::OUTPUT_NUM; ++i)
		temp[i] = Util::to_string(i);
	outputNeuron->setItems(temp);
	outputNeuron->setCurSelection(0);

	if(AgentUtil::clearStatBuffer)
		clearBuffer->check();
	else 
		clearBuffer->unCheck();

	temp.resize(16);
	temp[0] = "Max fintess";
	temp[1] = "Average fitness";
	temp[2] = "Max num of neurons";
	temp[3] = "Min mun of neurons";
	temp[4] = "Average num of neurons";
	temp[5] = "Max num of connections";
	temp[6] = "Min mun of connections";
	temp[7] = "Average mun of connections";
	temp[8] = "Best agnet av. delay time";
	temp[9] = "Dispers of best agent av delay time";
	temp[10] = "Average av. delay time";
	temp[11] = "Average dispers of av delay time";
	temp[12] = "Best agent network consistency";
	temp[13] = "Average network consistency";
	temp[14] = "Best agent result time";
	temp[15] = "Average network achieving result time";

	saveFromStatistic->clear();
	saveFromStatistic->setItems(temp);
	saveFromStatistic->setCurSelection(0);

	temp.resize(13);
	temp[0] = "Fintess";
	temp[1] = "Distance";
	temp[2] = "Activity";
	temp[3] = "netwConsistency";
	temp[4] = "Neurons num";
	temp[5] = "Connections num";
	temp[6] = "Max delay time";
	temp[7] = "Min delay time";
	temp[8] = "Av delay time";
	temp[9] = "Time delays for best agent";
	temp[10] = "Av time of achieving result";
	temp[11] = "Activation connection num";
	temp[12] = "AR connection num";

	saveFromNetworks->clear();
	saveFromNetworks->setItems(temp);
	saveFromNetworks->setCurSelection(0);
}

void ParamsController::updateParametersFromGui()
{
	char buff[32];
	generationNum->getString(buff,32);
	Parameters::GENERATION_NUM = atoi(buff);
	descendantNum->getString(buff,32);
	Parameters::DESCENDANT_NUM = atoi(buff);
	agentLifeTime->getString(buff,32);
	Parameters::ONE_AGENT_LIFE_TIME = atoi(buff);

	Parameters::typeOfFitness = fitnessType->getCurSelection();
	Parameters::typeOfControl = controlType->getCurSelection();

	angleStepSize->getString(buff,32);
	Parameters::ANGLE_STEP_SIZE = atof(buff);
	robotNumInOneSession->getString(buff,32);
	Parameters::PROCCESSES_NUM = atoi(buff);
	saveGenerationNum->getString(buff,32);
	Parameters::SAVE_GENERATION_NUM = atoi(buff);
	launchNum->getString(buff,32);
	Parameters::LAUNCH_NUM = atoi(buff);
	seed->getString(buff,32);
	Parameters::SEED = atoi(buff);
	naoNumInScene->getString(buff,32);
	Parameters::NAO_NUM_IN_SCENE = atoi(buff);
	Parameters::setDimentionOfInput(dimOfInput->getCurSelection());
	
	Parameters::autoloadParams = autoLoadParams->isChecked();
	Parameters::autosaveParams = autoSaveParams->isChecked();
	Parameters::autosaveNetworks = autoSaveNetwroks->isChecked();
	Parameters::autosaveStatistic = autoSaveNetwroks->isChecked();
	Parameters::showJointsData = showJointData->isChecked();

	Parameters::neuronType = neuronType->getCurSelection();
	Parameters::inputTransmissionFuncType = inputTransFuncType->getCurSelection();
	Parameters::intermTransmissionFuncType = intermTransFuncType->getCurSelection();
	Parameters::outTransmissionFuncType = outTransFuncType->getCurSelection();

	logLinearMax->getString(buff,32);
	Parameters::LOG_FUNC_LINEAR_MAX = atof(buff);
	logFuncKCoeff->getString(buff,32);
	Parameters::LOG_FUNC_K_COEFF = atof(buff);
	Parameters::useNormalization = useNormalization->isChecked();
	Parameters::useRandomAddition = useRandomAddition->isChecked();
	maxRandWeight->getString(buff,32);
	Parameters::MAX_RANDOM_WEIGHT = atof(buff);
	maxPotential->getString(buff,32);
	Parameters::MAX_POTENTIAL = atof(buff);
	minPotential->getString(buff,32);
	Parameters::MIN_POTENTIAL = atof(buff);
	activationThr->getString(buff,32);
	Parameters::ACTIVATION_THR = atof(buff);
	gauseBoundValue->getString(buff,32);
	Parameters::GAUSSE_BOUND_VALUE = atof(buff);
	uRand->getString(buff,32);
	Parameters::U_RAND = atof(buff);
	aRand->getString(buff,32);
	Parameters::A_RAND = atof(buff);
	resultThreshold->getString(buff,32);
	Parameters::RESULT_THR = atof(buff);
	delayTime->getString(buff,32);
	Parameters::NEURON_DELAY_TIME = atof(buff);
	outputMultiplyCoeff->getString(buff,32);
	Parameters::OUT_MILTIPLY_COEFF = atof(buff);
	sinFrequency->getString(buff,32);
	Parameters::SIN_FREQUENCY = atof(buff);

	Parameters::useReadyNetwork = useReadynetwork->isChecked();
	Parameters::readyNetworkType = readyNetworkType->getCurSelection();

	neuronsMatrix->getString(buff,32);
	Parameters::neuronsMatrix = string(buff);

	fullyNetNeuronNum->getString(buff,32);
	Parameters::FULLY_NETW_NEURON_NUM = atoi(buff);
	inputDelayNeuronNum->getString(buff,32);
	Parameters::INPUT_DELAY_NUM = atoi(buff);
	backFromOutputDelayNum->getString(buff,32);
	Parameters::BACK_FROM_OUTPUT_DELAY_NUM = atoi(buff);

	Parameters::evolutionAlgorithmType = evAlgType->getCurSelection();
	Parameters::descendantCreationType = descCreationType->getCurSelection();
	Parameters::selectionType = selectionType->getCurSelection();

	portionsOfChilds->getString(buff,32);
	Parameters::PORTION_OF_CHILDS = atof(buff);

	Parameters::selectFromStable = selectFromStable->isChecked();
	Parameters::mutateStablePartOfPopulation = mutateStablePart->isChecked();

	pWeightMut->getString(buff,32);
	Parameters::P_MUTATION_WEIGHT = atof(buff);
	pThresholdMut->getString(buff,32);
	Parameters::P_MUTATION_THRESHOLD = atof(buff);
	dispersWeight->getString(buff,32);
	Parameters::S_WEGHT = atof(buff);
	dispersThreshold->getString(buff,32);
	Parameters::S_THRESH = atof(buff);
	pDublication->getString(buff,32);
	Parameters::P_DUPLICATION = atof(buff);
	pDeleteion->getString(buff,32);
	Parameters::P_DELETION = atof(buff);
	pConnectionAdd->getString(buff,32);
	Parameters::P_CONNECTION_ADD = atof(buff);
	pConnectionRemove->getString(buff,32);
	Parameters::P_CONNECTION_REMOVE = atof(buff);
	pMutTime->getString(buff,32);
	Parameters::P_MUTATION_TIME = atof(buff);
	pMutResThr->getString(buff,32);
	Parameters::P_MUTATION_RES_THRESHOLD = atof(buff);
	dispersTime->getString(buff,32);
	Parameters::S_TIME_DELAY = atof(buff);
	dispersResThr->getString(buff,32);
	Parameters::S_RES_THRESH = atof(buff);
	pMutFrequency->getString(buff,32);
	Parameters::P_MUTATION_FREQUENCY = atof(buff);
	dispersFrequency->getString(buff,32);
	Parameters::S_FREQUENCY = atof(buff);

	startIntermNeurons->getString(buff,32);
	Parameters::START_INTERM_NEURON_NUM = atoi(buff);

	Parameters::addInhibitoryConnectionsToOutput = addInhibitoryConnectionToOtput->isChecked();
	Parameters::addBackConnectionToOutput = addBackConnectionToOtput->isChecked();

	Parameters::deleteInputNeurons = deleteInputNeurons->isChecked();
	Parameters::deleteOutputNeurons = deleteOutputNeurons->isChecked();
	Parameters::dublicateInputNeurons = dublicateInputNeurons->isChecked();
	Parameters::dublicateOutputNeurons = dublicateOutputNeurons->isChecked();

	AgentUtil::clearStatBuffer = clearBuffer->isChecked();
}


BOOL CALLBACK dpPage (HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;

	switch (nMsg)
	{
		case WM_COMMAND:
		{
			int id = LOWORD(wParam);
			mainCommand(LOWORD(wParam), hDlg, HIWORD (wParam));
			return TRUE;
		}
		break;
		case WM_CTLCOLORSTATIC:
		{
			hdc = (HDC)wParam;

			if(paramsController->console != NULL && (HWND)lParam == paramsController->console->hwnd())
			{
				SetTextColor (hdc, WHITE_CL);
				SetBkColor(hdc,GREY_CL);
				if(GREY_BR == NULL)
					GREY_BR = CreateSolidBrush(GREY_CL);
				return (LONG)GREY_BR;
			}
			else
			{
				SetBkColor(hdc,WHITE_CL);
				if(WHITE_BR == NULL)
					WHITE_BR = CreateSolidBrush(WHITE_CL);
				return (LONG)WHITE_BR;
			}
		}
		break;
		case WM_CTLCOLOREDIT:
		{
			hdc = (HDC)wParam;
			SetTextColor (hdc, WHITE_CL);
			if(GREY_BR == NULL)
				GREY_BR = CreateSolidBrush(GREY_CL);
			SetBkColor(hdc,GREY_CL);
			SetBkMode(hdc,TRANSPARENT);
			return (LONG)GREY_BR;
		}
		break;
		case WM_CTLCOLORDLG:
		{
			
			HDC hdc = (HDC)wParam;
			if(WHITE_BR == NULL)
				WHITE_BR = CreateSolidBrush(WHITE_CL);
			return (LONG)WHITE_BR;
		}
		break;
	}

	return FALSE;
} 

void setNewTab (HWND hDlg, INT nTab)
{
	for (INT i = 0; i < ARRAY_SIZE (g_hDialogs); ++ i)
	{
		if (i == nTab)
		{
			if (!IsWindowVisible (g_hDialogs [i]))
			{
				SetWindowPos (g_hDialogs [i], NULL, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER);
			}
		}
		else
		{
			if (IsWindowVisible (g_hDialogs [i]))
			{
				SetWindowPos (g_hDialogs [i], NULL, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER);
			}
		}
	}
}

Edit*  legendNum;
Edit*  legendName;
Button* renameLegButton;

BOOL CALLBACK RenameLegendDialogProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
    {
		case WM_INITDIALOG:
		{
			legendNum = new Edit(hwnd, IDC_EDIT_LEG_NUM);
			legendName = new Edit(hwnd, IDC_EDIT_LEG_NAME);
			renameLegButton = new Button(hwnd, IDC_BUTTON_RENAME_LEG_IN);
			return TRUE;
		}
		break;
   
		case WM_COMMAND:
		{
			int wid = LOWORD(wParam);
			if(LOWORD(wParam) == IDC_BUTTON_RENAME_LEG_IN)
			{
				//MessageBox(hwnd,"Rename Legend",NULL,MB_OK);
				char buff[32];
				legendNum->getString(buff,32);
				int num = atoi(buff);
				legendName->getString(buff,32);
				string name(buff);

				AgentUtil::renameLegend(num, name, m_Graph, paramsController->typeOfGraph->getCurSelection());
			}
			return TRUE;
		}
		break;
		case WM_CLOSE:
		{
			delete legendNum;
			delete legendName;
			delete renameLegButton;
			EndDialog(hwnd,0);
			return TRUE;
		}
		break;
    }
    return FALSE;
}

void createRenameLegendDialog()
{
	HWND hDialog = 0;
	//hDialog = CreateDialog(theInstance, MAKEINTRESOURCE (IDD_DIALOG_RENAME_LEGEND), NULL, (DLGPROC)RenameLegendDialogProc);
	DialogBox(theInstance,MAKEINTRESOURCE(IDD_DIALOG_RENAME_LEGEND),g_hDialogs [0],RenameLegendDialogProc);
}

void mainDialogInit (HWND hDlg)
{
	HICON hIcon = LoadIcon (theInstance, MAKEINTRESOURCE (IDI_ICON1));
    SendMessage (hDlg, WM_SETICON, WPARAM (TRUE), LPARAM (hIcon));
    hIcon = LoadIcon (theInstance, MAKEINTRESOURCE (IDI_ICON1));
    SendMessage (hDlg, WM_SETICON, WPARAM (FALSE), LPARAM (hIcon));

	//create tabs
	hTab = GetDlgItem (hDlg, IDC_TAB1);
	SetWindowLong (hTab, GWL_EXSTYLE, GetWindowLong (hTab, GWL_EXSTYLE) | WS_EX_CONTROLPARENT);

	{
		TCHAR sz [64] = { 0 };
		TCITEM tc = { 0 };
		tc.mask = TCIF_TEXT;
		tc.pszText = sz;

		for (INT i = 0; i < ARRAY_SIZE (g_hDialogs); ++ i)
		{
			LoadString (theInstance, IDS_HEADER_START + i, sz, ARRAY_SIZE (sz));
			TabCtrl_InsertItem (hTab, i, &tc);
		}
	}

	RECT rc = { 0 };
	GetClientRect (hTab, &rc);
	TabCtrl_AdjustRect (hTab, FALSE, &rc);

	for (INT i = 0; i < ARRAY_SIZE (g_hDialogs); ++ i)
	{
		g_hDialogs [i] = CreateDialog(theInstance, MAKEINTRESOURCE (g_nDialogs [i]), hTab, dpPage);
		SetWindowPos (g_hDialogs [i], NULL, rc. left, rc. top, rc. right - rc. left, rc. bottom - rc. top, SWP_HIDEWINDOW | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
	}

	//
	GetClientRect (g_hDialogs [3], &rc);
	rc.top += 75;
	m_Graph.Create(g_hDialogs [3], (HMENU)2012, rc);
	

	//create menu
	HMENU hMenu = LoadMenu(theInstance, MAKEINTRESOURCE(IDR_MENU2));
	SetMenu(hDlg,hMenu);

	if(paramsController == NULL)
		paramsController = new ParamsController ();

	for(int i=0; i<TAB_NUM;++i)
		paramsController->initializeControls(g_nDialogs[i], g_hDialogs[i]);

	
	if(!AgentUtil::loadDefaultParams())
		Parameters::setDefaultParameters();

	Parameters::programVersion = 3;

	paramsController->setParametersToGUI();

	paramsController->console->addMessage("Hello world!");

	setNewTab (hDlg, 0);
} 

BOOL CALLBACK DialogProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
		case WM_INITDIALOG:
		{
			mainDialogInit(hwnd);
			return TRUE;
		}
		break;
   
		case WM_NOTIFY:
		{
			LPNMHDR pStd = (LPNMHDR)lParam;
			if(pStd->code == TCN_SELCHANGE)
			{
				HWND hTab = GetDlgItem (hwnd, IDC_TAB1);
				setNewTab (hwnd, TabCtrl_GetCurSel (hTab));
			}
		}
		break;

		case WM_COMMAND:
		{
			int wID = LOWORD(wParam);
			mainCommand(wID,hwnd,0);
		}
		break;
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return TRUE;
		}
		break;
		case WM_CLOSE:
		{
			mainCommand(0,hwnd,WM_CLOSE);
			delete paramsController;
			DestroyWindow (hwnd);
			DeleteObject(GREY_BR);
			DeleteObject(WHITE_BR);
			return TRUE;
		}
		break;
    }
    return FALSE;
}

int realizeGUI(HINSTANCE hInst)
{
	InitCommonControls();

    HWND mainDialogHWND = 0;
	HWND hParent = GetDesktopWindow ();

    mainDialogHWND = CreateDialog(hInst, MAKEINTRESOURCE (IDD_DIALOG1), hParent, (DLGPROC)DialogProc);
    SetWindowPos (mainDialogHWND, NULL, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER);

    MSG  msg;
    int status;
    while ((status = GetMessage (&msg, 0, 0, 0)) != 0)
    {
        if (status == -1)
            return -1;
        if (!IsDialogMessage (mainDialogHWND, &msg))
        {
            TranslateMessage ( &msg );
            DispatchMessage ( &msg );
        }
    }

    return msg.wParam;
}