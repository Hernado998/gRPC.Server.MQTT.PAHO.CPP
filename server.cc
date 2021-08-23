#include <string>

#include <grpcpp/grpcpp.h>
#include "myServer.grpc.pb.h"

#include<unistd.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <thread>	
#include <atomic>
#include <list>
#include <chrono>
#include <cstring>
#include "mqtt/client.h"
#include "mqtt/properties.h"

using namespace std;
using namespace std::chrono;

vector <string> Brokers_List;
vector <string> Clients_List;
int serverState=0;

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using myserver::MqttServer;
using myserver::ConnectRequest;
using myserver::ConnectReply;
using myserver::DisconnectRequest;
using myserver::DisconnectReply;
using myserver::PublishRequest;
using myserver::PublishReply;
using myserver::SubscribeRequest;
using myserver::SubscribeReply;
using myserver::ShowBrokersRequest;
using myserver::ShowBrokersReply;
using myserver::ShowClientsRequest;
using myserver::ShowClientsReply;



// Message table function signature
using handler_t = std::function<bool(const mqtt::message&)>;

// Handler for data messages (i.e. topic "data/#")
bool data_handler(const mqtt::message& msg)
{
	cout << msg.get_topic() << ": " << msg.to_string() << endl;
	return true;
}

// Handler for command messages (i.e. topic "command")
// Return false to exit the application
bool command_handler(const mqtt::message& msg)
{
	if (msg.to_string() == "exit") {
		cout << "Exit command received" << endl;
		return false;
	}
	return true;
}



class ServerServiceImplementation final : public MqttServer::Service {

    Status showBrokersRequest(
        ServerContext* context, 
        const ShowBrokersRequest* request, 
        ShowBrokersReply* reply
    ) override {
        string msg ="List of Brokers connected to server:\n";
        for(int i=0;i<Brokers_List.size();i++){
            msg+= Brokers_List[i]+"\n";
        }
        reply->set_response(msg);
        return Status::OK; 
    }
    Status showClientsRequest(
        ServerContext* context, 
        const ShowClientsRequest* request, 
        ShowClientsReply* reply
    ) override {
        string msg ="List of Clients using the server:\n";
        for(int i=0;i<Clients_List.size();i++){
            msg+= Clients_List[i]+"\n";
        }
        reply->set_response(msg);
        return Status::OK; 
    }  

    Status connectionRequest(
        ServerContext* context, 
        const ConnectRequest* request, 
        ConnectReply* reply
    ) override {
        
        
        string adrs = request->adrs();
        string client_id = request->clientid();
        
        

        cout << "Initializing for server " << adrs << " ..." << endl;
		mqtt::client cli(adrs, client_id,
					 mqtt::create_options(MQTTVERSION_5));

	    auto connOpts = mqtt::connect_options_builder()
	    .mqtt_version(MQTTVERSION_5)
		.automatic_reconnect(seconds(2), seconds(30))
		.clean_session(false)
		.finalize();

        
        
        try
		{
			cout << "\nConnecting..." << endl;
			mqtt::connect_response rsp = cli.connect(connOpts);
            Brokers_List.push_back(adrs);
            Clients_List.push_back(client_id);
			cout << "  ...OK" << endl;	
		}
		catch (const mqtt::exception& exc) 
        {
			cerr << exc << endl;	
		}
        
        while(serverState==0){
            
        }
        cli.disconnect();
        reply->set_response("Disonnected !");
        return Status::OK;
        
        
    } 

    Status disconnectionRequest(
        ServerContext* context, 
        const DisconnectRequest* request, 
        DisconnectReply* reply
    ) override {
        
        
        string adrs = request->adrs();
        string client_id = request->clientid();

        mqtt::client cli(adrs, client_id,
					 mqtt::create_options(MQTTVERSION_5));

        auto connOpts = mqtt::connect_options_builder()
	    .mqtt_version(MQTTVERSION_5)
		.automatic_reconnect(seconds(2), seconds(30))
		.clean_session(false)
		.finalize();

        int state=0;

        for(int i=0;i<Brokers_List.size();i++){
            if(adrs==Brokers_List[i]){
                state=1;
                break;
            }
        }
        
        
        if(state==1){
            cout << "\nDisconnecting..." << endl;
            serverState=1;
		    
            
        
            reply->set_response("Out !");
        }
        else{
            reply->set_response("Server is not on !");
        }

        

        return Status::OK;
    } 

    Status subscribeRequest(
        ServerContext* context, 
        const SubscribeRequest* request, 
        SubscribeReply* reply
    ) override {
        string adrs = request->adrs();
        string topic = request->topic();
        string client_id = request->clientid();      
        mqtt::client cli(adrs, client_id,
					 mqtt::create_options(MQTTVERSION_5));

	    auto connOpts = mqtt::connect_options_builder()
	    .mqtt_version(MQTTVERSION_5)
		.automatic_reconnect(seconds(2), seconds(30))
		.clean_session(false)
		.finalize();


	    // Dispatch table to handle incoming messages based on Subscription ID's.
	    std::vector<handler_t> handler {
		    data_handler,
		    command_handler
	    };

	    
		    cout << "Connecting to the MQTT server..." << flush;
		    mqtt::connect_response rsp = cli.connect(connOpts);

		    cout << "OK\n" << endl;

		    if (!rsp.is_session_present()) {
			    std::cout << "Subscribing to topics..." << std::flush;

			    mqtt::subscribe_options subOpts;


			    mqtt::properties props1 {
				    { mqtt::property::SUBSCRIPTION_IDENTIFIER, 1 },
			    };
			    cli.subscribe(topic, 0, subOpts, props1);

			    mqtt::properties props2 {
				    { mqtt::property::SUBSCRIPTION_IDENTIFIER, 2 },
			    };
			    cli.subscribe(topic, 1, subOpts, props2);

			    std::cout << "OK" << std::endl;
		    }
		    else {
			    cout << "Session already present. Skipping subscribe." << std::endl;
		    }
            
            string s;
		    // Consume messages
	        /*while (true) {
                
		        auto msg = cli.consume_message();
               
                cout << msg->to_string() <<endl;
                
            
		        if (msg) {
                
				    // Get the subscription ID from the incoming message
			        int subId = mqtt::get<int>(msg->get_properties(),
										   mqtt::property::SUBSCRIPTION_IDENTIFIER);
                    //cout<<subId<<endl;

				    // Dispatch to a handler function based on the Subscription ID
			        if (!(handler[subId-1])(*msg))
				        break;
		        }
		        else if (!cli.is_connected()) {
			        cout << "Lost connection" << endl;
			        while (!cli.is_connected()) {
				        this_thread::sleep_for(milliseconds(250));
			        }
			        cout << "Re-established connection" << endl;
		        }
	        }*/
            mqtt::const_message_ptr msg;
            while(!msg){
                msg = cli.consume_message();
                //unsigned int microsecond = 1000000;
                //usleep(1 * microsecond);//sleeps for 1 second
                reply->set_response("New message from client with id: "+client_id+" & address: "+adrs+" under topic: "+topic+": \n"+msg->to_string());
            }
	    
	    
        
        return Status::OK;
    } 

    Status publishRequest(
        ServerContext* context, 
        const PublishRequest* request, 
        PublishReply* reply
    ) override {
        
        string address = request->adrs();

        string client_id = request->clientid(); 
        
        string topic = request->topic();
        
        string msg = request->message();

        
        

        mqtt::async_client cli(address, client_id);

        
        try
			{
				cout << "\nConnecting..." << endl;
				
				cli.connect()->wait();
				cout << "  ...OK" << endl;
				
			}
		catch (const mqtt::exception& exc) 
            {
				cerr << exc << endl;
				
			}

        cout << "\nPublishing..." << endl;
		
        mqtt::topic top(cli, topic, 0);
		mqtt::token_ptr tok;
		tok = top.publish(msg);
		tok->wait();
        reply->set_response("Published: "+msg+"\n"+"On topic: "+topic+"\n"+"Broker address: "+address);	
        

        return Status::OK;
    }
};

void Run() {
    std::string address("0.0.0.0:5000");
    ServerServiceImplementation service;

    ServerBuilder builder;

    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on port: " << address << std::endl;

    server->Wait();
}

int main(int argc, char** argv) {
    Run();

    return 0;
}