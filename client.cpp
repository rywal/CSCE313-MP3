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
/* LOCAL FUNCTIONS -- INDIVIDUAL REQUESTS */
/*--------------------------------------------------------------------------*/

int local_cwrite(string _msg) {
    
    if (_msg.length() >= 255) {
        cerr << "Message too long for Channel!\n";
        return -1;
    }
    
    //  cout << "Request Channel (" << my_name << ") writing [" << _msg << "]";
    
    const char * s = _msg.c_str();
    
//    if (write(0, s, strlen(s)+1) < 0) {
//        perror(string("Request Channel ( some ) : Error writing to pipe!").c_str());
//    }
    
    return 1;
    //  cout << "(" << my_name << ") done writing." << endl;
}

void local_process_hello(RequestChannel & _channel, const string & _request) {
    //_channel.cwrite("hello to you too");
    cout << "LOCAL: hello to you too" << endl;
}

void local_process_data(RequestChannel & _channel, const string &  _request) {
    //int2string(rand() % 100);
    local_cwrite(int2string(rand() % 100));
}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THE PROCESS REQUEST LOOP */
/*--------------------------------------------------------------------------*/

void local_process_request(RequestChannel & _channel, const string & _request) {
    
    if (_request.compare(0, 5, "hello") == 0) {
        local_process_hello(_channel, _request);
    }
    else if (_request.compare(0, 4, "data") == 0) {
        local_process_data(_channel, _request);
    }
    else {
        local_cwrite("unknown request");
    }
    
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {

    int pid = fork();
    if (pid == 0) {
        //this process is the 'child', so run the dataserver
        system("./dataserver");
    } else {
        int num_requests = 10000;
        long long int request_times[num_requests];
        long long int local_request_times[num_requests];
        
        cout << "CLIENT STARTED:" << endl;

        cout << "Establishing control channel... " << flush;
        RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
        cout << "done." << endl;
        
        // Declare execution time handling variables
        high_resolution_clock::time_point pre_request_time,         // Time at start of server request
                                          post_request_time,        // Time at end of server request
                                          pre_local_request_time,   // Time at start of local request
                                          post_local_request_time;  // Time at end of local request
        double run_time;
        double local_run_time;

        /* -- Start sending a sequence of requests */

        string reply1 = chan.send_request("hello");
        cout << "Reply to request 'hello' is '" << reply1 << "'" << endl;

        string reply2 = chan.send_request("data Joe Smith");
        cout << "Reply to request 'data Joe Smith' is '" << reply2 << "'" << endl;

        string reply3 = chan.send_request("data Jane Smith");
        cout << "Reply to request 'data Jane Smith' is '" << reply3 << "'" << endl;

        for(int i = 0; i < num_requests; i++) {
            string request_string("data TestPerson" + int2string(i));
            
            // Send request to dataserver
            pre_request_time = high_resolution_clock::now();
            string reply_string = chan.send_request(request_string);
            post_request_time = high_resolution_clock::now();
            
            // Capture dataserver execution time and same to array
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>( post_request_time - pre_request_time ).count();
            run_time += duration;
            request_times[i] = duration;
            
            // Print out the details of the request response
            cout << "reply to request took " << duration << " microseconds round-trip " << i << ":" << reply_string << endl;;
            
            // Handle request locally as if it were the dataserver
            pre_local_request_time = high_resolution_clock::now();
            local_process_request(chan, request_string);
            post_local_request_time = high_resolution_clock::now();
            
            // Calculate the time it takes, in order to compare with server request
            auto local_duration = std::chrono::duration_cast<std::chrono::microseconds>( post_local_request_time - pre_local_request_time ).count();
            local_run_time += local_duration;
            local_request_times[i] = local_duration;
        }
        
        string reply4 = chan.send_request("quit");
        cout << "Reply to request 'quit' is '" << reply4 << endl;

        usleep(1000000);
        
        // Calculate time related information for server requests
        long double average_time = 0.0;
        for(int i = 0; i < num_requests; i++){
            average_time += request_times[i];
        }
        average_time = average_time/num_requests;
        
        long double standard_deviation = 0.0;
        for(int i = 0; i < num_requests; i++){
            standard_deviation += ((request_times[i]-average_time)*(request_times[i]-average_time));
        }
        standard_deviation = sqrt(standard_deviation/(num_requests-1));
        
        // Print out server request results
        cout << " -------------------------------------------- " << endl << flush
             << "|                Server Results              |" << endl << flush
             << "|--------------------------------------------|" << endl << flush
             << "| Run-time:         |" << setw(20) << run_time/1000 << " ms |" << endl << flush
             << "| # of requests     |" << setw(23) << num_requests << " |" << endl << flush
             << "| Avg. Request Time |" << setw(23) << average_time << " |" << endl << flush
             << "| Stand. Deviation  |" << setw(23) << standard_deviation << " |" << endl << flush
             << " -------------------------------------------- " << endl << flush;
        
        // Calculate the time for local requests
        long double local_average_time = 0.0;
        for(int i = 0; i < num_requests; i++){
            local_average_time += local_request_times[i];
        }
        local_average_time = local_average_time/num_requests;
        
        long double local_standard_deviation = 0.0;
        for(int i = 0; i < num_requests; i++){
            local_standard_deviation += ((local_request_times[i]-local_average_time)*(local_request_times[i]-local_average_time));
        }
        local_standard_deviation = sqrt(local_standard_deviation/(num_requests-1));
        
        // Print out local request results
        cout << " -------------------------------------------- " << endl << flush
             << "|                Local Results               |" << endl << flush
             << "|--------------------------------------------|" << endl << flush
             << "| Run-time:         |" << setw(20) << local_run_time/1000 << " ms |" << endl << flush
             << "| # of requests     |" << setw(23) << num_requests << " |" << endl << flush
             << "| Avg. Request Time |" << setw(23) << local_average_time << " |" << endl << flush
             << "| Stand. Deviation  |" << setw(23) << local_standard_deviation << " |" << endl << flush
             << " -------------------------------------------- " << endl << flush;
    }
}
