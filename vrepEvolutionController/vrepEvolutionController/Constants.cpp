#include "Constants.h"

const double Constants::PI = 3.14;

const int Constants::RECCURENT_NETWORK_TYPE = 0;

//types of ready networks
const int Constants::FEEDFORWARD_NETWORK_TYPE = 0;
const int Constants::FULLY_CONNECTED_NETWORK = 1;
const int Constants::JORDAN_SIMILAR_NETWORK = 2;

//types of neurons with differnt dynamics
const int Constants::SIMPLE_NEURON = 0;
const int Constants::TIME_DELAY_NEURON = 1;
const int Constants::AR_NEURON = 2;
const int Constants::LEAKY_INTEGRATOR_NEURONS = 3;

const int Constants::INPUTE_NEURON_TYPE = 0;
const int Constants::INTERM_NEURON_TYPE = 1;
const int Constants::OUTPUT_NEURON_TYPE = 2;

const int Constants::LOGISTIC_FUNCTION = 0;
const int Constants::THRESHOLD_FUNCTION = 1;
const int Constants::GAUSS_FUNCTION = 2;
const int Constants::LOG_AND_THRESHOLD_FUNCTION = 3;
const int Constants::SIN_FUNCTION = 4;
const int Constants::NONE_FUNCTION = 5;

const int Constants::SIMPLE_CONNECTION = 0;
const int Constants::AR_CONNECTION = 1;

const int Constants::LEFT_KEY = 314;
const int Constants::RIGHT_KEY = 316;
const int Constants::UP_KEY = 315;
const int Constants::DOWN_KEY = 315;
const int Constants::ONE_KEY = 49;
const int Constants::TWO_KEY = 50;
const int Constants::THREE_KEY = 51;
const int Constants::ENTER_KEY = 4;

//types of evolutionary algorithms
const int Constants::GENETIC_ALG = 0;
const int Constants::EVOLUTIONARY_STRATEGY = 1;
//types of creation descendant
const int Constants::CREATE_DESC_WITH_CROSSOVER = 0;
const int Constants::CREATE_DESC_MB = 1;
const int Constants::CREATE_DESC_MB_2 = 2;
const int Constants::CREATE_DESC_FOR_HARMONIC_CONTROL = 3;
//..neat, aeg e.t.c

const int Constants::FULL_DIM = 0;
const int Constants::MIDDLE_DIM = 1;

//types of fitnesses
const int Constants::DISTANCE_FTNESS = 0;
const int Constants::ACTIVITY_FTNESS = 1;
const int Constants::MIX_FTNESS = 2;
const int Constants::HEIGHT_DISTANCE_FTNESS = 3;

//types of joint control
const int Constants::COMPETITION_CONTROL = 0;
const int Constants::DELTA_COMPETTION_CONTROL = 1;
const int Constants::PROP_COMPETTION_CONTROL = 2;
const int Constants::ONE_NEURON_CONTROL = 3;
const int Constants::MULTIPLE_COMPETTITION_CONTROL = 4;
const int Constants::FORCE_COTROL = 5;
const int Constants::HARMONIC_CONTROL = 6;

//commands from main controller and to him (for inner logic, command main corresponde some count of events)
const int Constants::INITIALIZE_CYCLE_MESSAGE = 0;
const int Constants::START_LIFE_MESSAGE = 1;
const int Constants::AGENT_IS_READY_MESSAGE = 2;
const int Constants::AGENT_LIFE_ENDED_MESSAGE = 3;
const int Constants::EXIT_PROCESS_MESSAGE = 4;
const int Constants::AGENT_PROCESS_CLEARED_MESSAGE = 5;

const string Constants::wbt_file_content = "#VRML_SIM V6.0 utf8\nWorldInfo {\n info \n []\n title \"Aldebaran's Nao\"\n basicTimeStep 40 \n displayRefresh 1 \n runRealTime TRUE \n}\n Viewpoint {\n orientation -0.180133 0.962824 0.201299 1.60587 \n position 6.15548 2.56266 -0.483624\n}\n Background {}\n DirectionalLight {\n ambientIntensity 1 \n direction -1 -1 0.1 \n castShadows TRUE \n}\n Floor {\n size 5 5 \n scale 5 5 \n}\n Supervisor {\n children [\n Emitter {\n}  \n Receiver {\n} \n ] \n  controller \"Superviser\" \n} \n DEF AGENT Nao_H25_V40 {\n rotation 0 0 1 -1.57 controllerArgs \"";

const string Constants::READ_NETWORK_AND_RESET = "UPDATE_NETWORK_AND_RESET";
const string Constants::RESET_AND_STOP = "RESET_AND_STOP";
const string Constants::AGENT_IS_READY = "AGENT_IS_READY";
const string Constants::START_LIFE = "START_LIFE";
const string Constants::AGENT_LIFE_ENDED = "AGENT_LIFE_ENDED";
const string Constants::EXIT_PROCESS = "EXIT_PROCESS";
const string Constants::AGENT_PROCESS_CLEARED = "AGENT_PROCESS_CLEARED";

const string Constants::BASE_FILE_NAME = "AGENT_FILE";

const int Constants::SHARED_MEMORY_SIZE = 50*1024*1024;//100mb

const int Constants::INT_SIZE = sizeof(int);
const int Constants::DOUBLE_SIZE = sizeof(double);
const int Constants::BOOL_SIZE = sizeof(bool);

const int Constants::MAX_FITNESS_PLOT = 0;
const int Constants::AVERAGE_FITNESS_PLOT = 1;
const int Constants::MAX_NEURONS_PLOT = 2;
const int Constants::MIN_NEURONS_PLOT = 3;
const int Constants::AVERAGE_NEURONS_PLOT = 4;
const int Constants::MAX_CONNECTIONS_PLOT = 5;
const int Constants::MIN_CONNECTIONS_PLOT = 6;
const int Constants::AVERAGE_CONNECTIONS_PLOT = 7;
const int Constants::BEST_AGENT_NET_CONSISTENSY_PLOT = 8;
const int Constants::AVERAGE_NET_CONSISTENSY_PLOT = 9;
const int Constants::BEST_AGENT_RESULT_TIME_PLOT = 10;
const int Constants::AVERAGE_RESULT_TIME_PLOT = 11;
const int Constants::BEST_AGENT_AV_DELAY_TIME = 12;
const int Constants::AVERAGE_AV_DELAY_TIME = 13;
const int Constants::DISPERS_BEST_AGENT_AV_DELAY_TIME = 14;
const int Constants::DISPERS_OF_AVAREGE_AV_DELAY_TIME = 15;

const int Constants::STATISTIC_TYPE_GRAPH = 0;
const int Constants::JOINT_TYPE_GRAPH = 1;
const int Constants::NETWORK_TYPE_GRAPH = 2;

const int  Constants::FITNESS_SAVE = 0;
const int  Constants::DISTANCE_SAVE = 1;
const int  Constants::ACTIVITY_SAVE = 2;
const int  Constants::NET_CONSISTENCY_SAVE = 3;
const int  Constants::NEURONS_NUM_SAVE = 4;
const int  Constants::CONNECTIONS_NUM_SAVE = 5;
const int  Constants::MAX_DELAY_SAVE = 6;
const int  Constants::MIN_DELAY_SAVE = 7;
const int  Constants::AV_DELAY_SAVE = 8;
const int  Constants::DELAY_FOR_BEST_AG_SAVE = 9;
const int  Constants::AV_TIME_OF_ACHIEVING_RESULT_SAVE = 10;
const int  Constants::ACTIVATION_CONNECTION_NUM_SAVE = 11;
const int  Constants::AR_CONNECTION_NUM_SAVE = 12;

const int  Constants::READ_NETWORK_AND_PLAY_SHOW_MESS = 0;
const int  Constants::EXIT_AGENT_SHOW_MESS = 1;

const int Constants::IDLE_MODE = 0;
const int Constants::CALCULATION_MODE = 1;
const int Constants::SHOW_MODE = 2;


const int Constants::ROULETTE_SEL_TYPE = 0;
const int Constants::RANDOM_SEL_TYPE = 1;