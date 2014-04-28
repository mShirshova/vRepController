#ifndef GUI_UTIL_H
#define	GUI_UTIL_H

#include <windows.h>
#include <iostream>
#include <string>
#include <new.h>

#include "resource.h"
#include "Controls.h"
#include "AzGraphCtrl.h"
#include "Util.h"
#include "Generation.h"
#include "AgentUtil.h"

class ParamsController
{
public:
	ParamsController(){
		_initilizedPageCount = 0;

		generationNum = NULL;
		descendantNum = NULL;
		agentLifeTime = NULL;
		fitnessType = NULL;
		controlType = NULL;

		angleStepSize = NULL;
		dimOfInput = NULL;
		robotNumInOneSession = NULL;
		saveGenerationNum = NULL;
		seed = NULL;
		naoNumInScene = NULL;

		launchNum = NULL;
		autoLoadParams = NULL;
		autoSaveParams = NULL;
		autoSaveNetwroks = NULL;
		autoSaveStatistic = NULL;
		showJointData = NULL;

		neuronType = NULL;
		inputTransFuncType = NULL;
		intermTransFuncType = NULL;
		outTransFuncType = NULL;
		logLinearMax = NULL;
		logFuncKCoeff = NULL;
		useNormalization = NULL;
		useRandomAddition = NULL;
		maxRandWeight = NULL;
		maxPotential = NULL;
		minPotential = NULL;
		activationThr = NULL;
		gauseBoundValue = NULL;
		uRand = NULL;
		aRand = NULL;
		resultThreshold = NULL;
		delayTime = NULL;
		outputMultiplyCoeff = NULL;
		sinFrequency = NULL;
		startIntermNeurons = NULL;

		addInhibitoryConnectionToOtput = NULL;
		addBackConnectionToOtput = NULL;

		useReadynetwork = NULL;
		readyNetworkType = NULL;

		neuronsMatrix = NULL;
	
		fullyNetNeuronNum = NULL;
		inputDelayNeuronNum = NULL;
		backFromOutputDelayNum = NULL;

		evAlgType = NULL;
		descCreationType = NULL;
		selectionType = NULL;
		selectFromStable = NULL;
		mutateStablePart = NULL;
		portionsOfChilds = NULL;
		pWeightMut = NULL;
		pThresholdMut = NULL;
		dispersWeight = NULL;
		dispersThreshold = NULL;
		pDublication = NULL;
		pDeleteion = NULL;
		pConnectionAdd = NULL;
		pConnectionRemove = NULL;
		pMutResThr = NULL;
		pMutTime = NULL;
		dispersTime = NULL;
		dispersResThr = NULL;
		pMutFrequency = NULL;
		dispersFrequency = NULL;

		dublicateInputNeurons = NULL;
		dublicateOutputNeurons = NULL;
		deleteInputNeurons = NULL;
		deleteOutputNeurons = NULL;

		console = NULL;
		selectResults = NULL;
		typeOfGraph = NULL;
		graphDescNum = NULL;
	    typeOfJoint= NULL;
		outputNeuron = NULL;
		clearBuffer = NULL;
		renameLegend = NULL;

		saveFromStatistic = NULL;
		saveFromNetworks = NULL;
		saveFromNetworksButton = NULL;
		saveFromStatisticButton = NULL;
		saveForBestAgent = NULL;

	};
    ~ParamsController ();

	void initializeControls(int pageID, HWND hwnd);
	void updateParametersFromGui();
	void setParametersToGUI();
	bool isAllInitialized();

	Edit*        generationNum;
	Edit*       descendantNum;
	Edit*        agentLifeTime;
	MComboBox*    fitnessType;
	MComboBox*    controlType;

	Edit*        angleStepSize;
	MComboBox*   dimOfInput;
	Edit*        robotNumInOneSession;
	Edit*        saveGenerationNum;
	Edit*        seed;
	Edit*        naoNumInScene;
	
	Edit*        launchNum;
	CheckBox*    autoLoadParams;
	CheckBox*    autoSaveParams;
	CheckBox*    autoSaveNetwroks;
	CheckBox*    autoSaveStatistic;
	CheckBox*    showJointData;
	
	MComboBox*   neuronType;
	MComboBox*   inputTransFuncType;
	MComboBox*   intermTransFuncType;
	MComboBox*   outTransFuncType;
	Edit*        logLinearMax;
	Edit*        logFuncKCoeff;
	CheckBox*    useNormalization;
	CheckBox*    useRandomAddition;
	Edit*        maxRandWeight;
	Edit*        maxPotential;
	Edit*        minPotential;
	Edit*        activationThr;
	Edit*        gauseBoundValue;
	Edit*        uRand;
	Edit*        aRand;
	Edit*        resultThreshold;
	Edit*        delayTime;
	Edit*        outputMultiplyCoeff;
	Edit*        sinFrequency;
	Edit*        startIntermNeurons;

	CheckBox*    addInhibitoryConnectionToOtput;
	CheckBox*    addBackConnectionToOtput;

	CheckBox*    useReadynetwork;
	MComboBox*   readyNetworkType;

	Edit*        neuronsMatrix;
	
	Edit*        fullyNetNeuronNum;
	Edit*        inputDelayNeuronNum;
	Edit*        backFromOutputDelayNum;

	MComboBox*    evAlgType;
	MComboBox*    descCreationType;
	MComboBox*    selectionType;
	CheckBox*    selectFromStable;
	CheckBox*    mutateStablePart;
	Edit*        portionsOfChilds;
	Edit*        pWeightMut;
	Edit*        pThresholdMut;
	Edit*        dispersWeight;
	Edit*        dispersThreshold;
	Edit*        pDublication;
	Edit*        pDeleteion;
	Edit*        pConnectionAdd;
	Edit*        pConnectionRemove;
	Edit*        pMutResThr;
	Edit*        pMutTime;
	Edit*        dispersTime;
	Edit*        dispersResThr;
	Edit*        pMutFrequency;
	Edit*        dispersFrequency;

	CheckBox*    dublicateInputNeurons;
	CheckBox*    dublicateOutputNeurons;
	CheckBox*    deleteInputNeurons;
	CheckBox*    deleteOutputNeurons;


	Edit*        console;
	MComboBox*   selectResults;
	MComboBox*   typeOfGraph;
	Edit*        graphDescNum;
	MComboBox*   typeOfJoint;
	MComboBox*   outputNeuron;
	CheckBox*    clearBuffer;
	Button*      renameLegend;

	MComboBox*	 saveFromStatistic;
	MComboBox*   saveFromNetworks;
	Button*      saveFromStatisticButton;
	Button*      saveFromNetworksButton;
	Button*      saveForBestAgent;
protected:
	int _initilizedPageCount;
};

extern ParamsController* paramsController;
extern AzGraphCtrl m_Graph;

int realizeGUI(HINSTANCE hInst);
void mainCommand(int id, HWND hwnd, int command);
void saveSomeFile(char (&fileName)[MAX_PATH], char* lpstrFilter, char* lpstrTitle);
void saveGraph();
void saveAgents(Generation& g);
void loadAgents(Generation& g);
void saveBestAgent(Generation& g);
void loadBestAgent(Generation& g);
void saveParams();
void loadParams();
void saveResults();
void saveAvResults();
void loadResults();

void createRenameLegendDialog();

#endif