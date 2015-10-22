/* 
    File: simpleclient.cpp

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/11

    Simple client main program for MP2 in CSCE 313
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#include <unistd.h>

#include <chrono> // Added to measure time of requests

#include "reqchannel.h"

using namespace std;
using namespace std::chrono;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- SUPPORT FUNCTIONS */
/*--------------------------------------------------------------------------*/

string int2string(int number) {
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {

    int pid = fork();
    if (pid == 0) {
        // this process is the 'child', so run the dataserver
        system("./dataserver");
    } else {
        int num_requests = 5;
        long long int request_times[num_requests];
        
        cout << "CLIENT STARTED:" << endl;

        cout << "Establishing control channel... " << flush;
        RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
        cout << "done." << endl;
        
        high_resolution_clock::time_point pre_request_time,         // Time at start of server request
                                          post_request_time,        // Time at end of server request
                                          pre_local_request_time,   // Time at start of local request
                                          post_local_request_time;  // Time at end of local request
        long long int run_time;
        


        /* -- Start sending a sequence of requests */

        string reply1 = chan.send_request("hello");
        cout << "Reply to request 'hello' is '" << reply1 << "'" << endl;

        string reply2 = chan.send_request("data Joe Smith");
        cout << "Reply to request 'data Joe Smith' is '" << reply2 << "'" << endl;

        string reply3 = chan.send_request("data Jane Smith");
        cout << "Reply to request 'data Jane Smith' is '" << reply3 << "'" << endl;

        for(int i = 0; i < num_requests; i++) {
            string request_string("data TestPerson" + int2string(i));
            
            pre_request_time = high_resolution_clock::now();
            string reply_string = chan.send_request(request_string);
            post_request_time = high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>( post_request_time - pre_request_time ).count();
            run_time += duration;
            
            request_times[i] = duration;
            
            cout << "reply to request took " << duration << " microseconds round-trip " << i << ":" << reply_string << endl;;
        }
        
        string reply4 = chan.send_request("quit");
        cout << "Reply to request 'quit' is '" << reply4 << endl;

        usleep(1000000);
        
        
        float average_time = 0.0;
        for(int i = 0; i < num_requests; i++){
            average_time += request_times[i];
        }
        average_time = average_time/num_requests;
        
        float standard_deviation = 0.0;
        for(int i = 0; i < num_requests; i++){
            standard_deviation += ((request_times[i]-average_time)*(request_times[i]-average_time));
        }
        standard_deviation = sqrt(standard_deviation/(num_requests-1));
        cout << " -------------------------------------------- " << endl;
        cout << "|                   Results                  |" << endl;
        cout << "|--------------------------------------------|" << endl;
        cout << "| Run-time:         |" << setw(10) << run_time/1000 << " milliseconds |" << endl;
        cout << "| # of requests     |" << setw(23) << num_requests << " |" << endl;
        cout << "| Avg. Request Time |" << setw(23) << average_time << " |" << endl;
        cout << "| Stand. Deviation  |" << setw(23) << standard_deviation << " |" << endl;
        cout << " -------------------------------------------- " << endl;
    }
}
