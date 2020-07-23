#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <unordered_map>
#include <unordered_set>

typedef std::unordered_set<std::string> set_str; 
typedef std::unordered_map<std::string, set_str> adj_list;

// main task to perform 
std::string task;

// tasks in the order they should be executed
std::vector<std::string> ordered_tasks;

// graph in form of adjacency list reflecting dependencies between tasks
adj_list dependencies;

// mapping between names of the tasks and their commands
std::unordered_map<std::string, std::string> cmds;

// helper objects
set_str seen;
set_str subgraph_nodes;


/**
 * put names of the tasks in set and return it.
 *
 * @param s string with comma-separated names of tasks
 * @return set containing names of tasks
 */
set_str parseDepends(const std::string& s) {
    char delim = ',';
    set_str result;
    std::stringstream ss (s);
    std::string item;

    while (getline (ss, item, delim)) {
        result.insert(item);
    }

    return result;
}


/**
 * traverse the graph in depth first search manner and
 * collect the encountered nodes in subgraph_nodes. if
 * cycle found or a node does not exist, throw an exception
 *
 * @param root node to traverse
 */
void DFS(const std::string& root){
    if (seen.count(root)){
        std::cout << "Error! circular dependency." << std::endl;
        throw 7;
    }

    if (!dependencies.count(root)){
        std::cout << "Error! required task '" << root 
                  <<"' does not exist." << std::endl;
        throw 8;
    }

    seen.insert(root);
    subgraph_nodes.insert(root);

    for (auto const &n: dependencies[root]){
        DFS(n);
    }

    seen.erase(root);
}


/**
 * do topological sort. tasks added to ordered_tasks
 * in the order they should be executed
 *
 */
void topologicalSort(){
    // creata subgraph as we only need to sort the part of the graph
    // that has the task to perform as a root
    adj_list subgraph;

    for (auto const &n: subgraph_nodes){
        subgraph[n] = dependencies[n];
    }

    // nodes with no incoming edges
    std::queue<std::string> nodes_with_no_income;
    for (auto const &n: subgraph){
        if (!n.second.size()){
            nodes_with_no_income.push(n.first);
        }
    }

    while ( nodes_with_no_income.size() ){
        std::string curr = nodes_with_no_income.front();
        nodes_with_no_income.pop();
        ordered_tasks.push_back(curr);

        for (auto& n: subgraph){
            if (n.second.count(curr)){
                n.second.erase(curr);

                if (!n.second.size()) {
                    nodes_with_no_income.push(n.first);
                }
            }
        }
    }
}


/**
 * exceute the tasks
 *
 */
void executeTask(){
    for(auto const &t: ordered_tasks){
        system(cmds[t].c_str());
    }
}


/**
 * parse command line. read the name of the task to perform. Open
 * files and read in the tasks (names, commands, dependencies)
 *
 * @param argc number of command line parameters
 * @param argv command line parameters
 */
void parseCmdLine(int argc, char* argv[]){

    if (argc < 3) {
        std::cout << "Usage: " << argv[0]
                  << " <task_to_be_done> [TASK_FILE_0 TASK_FILE_1 ...]"
                  << std::endl;
        throw 1;
    }

    // name of the task to perform
    task = std::string(argv[1]);

    for (int i = 2; i < argc; i++){

        std::string task_name, cmd;
        std::ifstream myfile ( argv[i] );

        if (myfile.is_open())
        {
            // read task name
            if ( !getline (myfile, task_name) ){
                std::cout << "Error! No task name in the file." << std::endl;
                throw 2;
            }
            // read task command
            if ( !getline (myfile, cmd) ){
                std::cout << "Error! No task command in the file." << std::endl;
                throw 3;
            }

            // task already defined in another file
            if (cmds.count(task_name)){
                std::cout << "Error! Task defined in another file." << std::endl;
                throw 4;
            }

            cmds[task_name] = cmd;

            // read task dependencies
            if ( !getline (myfile, cmd) ){
                dependencies[task_name] = set_str();
            }
            else {
                dependencies[task_name] = parseDepends(cmd);
            }

            myfile.close();
        }
        else{
            std::cout << "Error! Unable to open file.";
            throw 5;
        }

    }

    // task definition not found in the files
    if (!dependencies.count(task)){
        std::cout << "Error! Main task definition not found." << std::endl;
        throw 6;
    }
}


int main(int argc, char* argv[])
{
    try {
        parseCmdLine(argc, argv);
        DFS(task);
        topologicalSort();
        executeTask();
    }
    catch (...){
    }

    return 0;
}
