#include <string>
#include <cstring>
#include <grpcpp/grpcpp.h>
#include "myServer.grpc.pb.h"

#include<unistd.h>

using namespace std;
using grpc::Channel;
using grpc::ClientContext;
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

class ClientSide {
    public:
        ClientSide(std::shared_ptr<Channel> channel) : stub_(MqttServer::NewStub(channel)) {}

    string showBrokersRequest(string req){
        ShowBrokersRequest request;
        ShowBrokersReply reply;

        request.set_req(req);

        ClientContext context;

        Status status = stub_->showBrokersRequest(&context, request, &reply);
        if(status.ok()){
                return reply.response();
            }
        else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return "None";
        }
    }
    string showClientsRequest(string req){
        ShowClientsRequest request;
        ShowClientsReply reply;

        request.set_req(req);

        ClientContext context;

        Status status = stub_->showClientsRequest(&context, request, &reply);
        if(status.ok()){
                return reply.response();
            }
        else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return "None";
        }
    }
    string connectionRequest(string adrs, string clientID) {
        ConnectRequest request;
        DisconnectRequest disc_request;
        
        request.set_adrs(adrs);
        request.set_clientid(clientID);
        disc_request.set_adrs(adrs);
        
        ConnectReply reply;
        DisconnectReply disc_reply;
        cout<<"Connecting to broker: "+adrs+" /id: "+clientID+" ..."<<endl; 
        ClientContext context;
        ClientContext context1;

        Status status = stub_->connectionRequest(&context, request, &reply);
        
        if(status.ok()){
            cout<<reply.response()<<endl;
            cout<<"Type exit to quit !"<<endl;
            string v;
            cin>>v;
            string response;
            while(v!="exit"){
                cin>>v;
            }
            Status status1 = stub_->disconnectionRequest(&context1, disc_request, &disc_reply);
            
            if(status1.ok()){
                return disc_reply.response();
            }
            else {
            std::cout << status1.error_code() << ": " << status1.error_message() << std::endl;
            return "None";
            }
        } 
        else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return "None";
        }
    }
    string disconnectionRequest(string adrs, string clientID) {
        DisconnectRequest request;

        request.set_adrs(adrs);
        request.set_clientid(clientID);

        DisconnectReply reply;

        ClientContext context;

        Status status = stub_->disconnectionRequest(&context, request, &reply);

        if(status.ok()){
            return reply.response();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return "None";
        }
    }

    string subscribeRequest(string adrs, string topic, string clientID) {
        SubscribeRequest request;

        
        request.set_adrs(adrs);
        request.set_topic(topic);
        request.set_clientid(clientID);
        

        SubscribeReply reply;

        ClientContext context;

        Status status = stub_->subscribeRequest(&context, request, &reply);

        if(status.ok()){
            return reply.response();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return "None";
        }
    }

    string publishRequest(string adrs, string topic,string msg, string clientID) {
        PublishRequest request;

        request.set_adrs(adrs);
        request.set_topic(topic);
        request.set_message(msg);
        request.set_clientid(clientID);
        

        PublishReply reply;

        ClientContext context;

        Status status = stub_->publishRequest(&context, request, &reply);

        if(status.ok()){
            return reply.response();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return "None";
        }   
    }

    private:
        std::unique_ptr<MqttServer::Stub> stub_;
};

void Run() {
    std::string address("0.0.0.0:5000");
    ClientSide client(
        grpc::CreateChannel(
            address, 
            grpc::InsecureChannelCredentials()
        )
    );
    string adrs="0.0.0.0:1883";
    string topic="test";
    string msg = "Hello Friend !";
    string client_id="client_00";

    int v;
    std::cout<<"Press 1 to connect\nPress 2 to disconnect\nPress 3 to subscribe\nPress 4 to publish\nPress 5 to show brokers connected to the server\nPress 6 to show clients using the server"<<std::endl;
    std::cin>>v;
    string response;
    switch (v)
    {
    case 1:
    {
        std::cout<<"Broker address:"<<std::endl;
        string s0;
        cin>>s0;
        std::cout<<"This device id:"<<std::endl;
        string s1;
        cin>>s1;
        response = client.connectionRequest(s0,s1);
        std::cout<< response<<std::endl;
        break;
    }
    case 2:
    {
        std::cout<<"Broker address:"<<std::endl;
        string s0;
        cin>>s0;
        std::cout<<"This device id:"<<std::endl;
        string s1;
        cin>>s1;
        response = client.disconnectionRequest(s0,s1);
        std::cout<< response<<std::endl;
        break;
    }
    case 3:
    {
        std::cout<<"Broker address:"<<std::endl;
        string s0;
        cin>>s0;
        std::cout<<"This device id:"<<std::endl;
        string s1;
        cin>>s1;
        std::cout<<"write a topic:"<<std::endl;
        string s2;
        cin>>s2;
        std::cout<<"Waiting for messages ..."<<std::endl;
        while(true){
            response = client.subscribeRequest(s0,s2,s1);
            std::cout<< response<<std::endl;
            std::cout<<"Waiting for messages ..."<<std::endl; 
        }
        break;
    }
    case 4:
    {
        std::cout<<"Broker address:"<<std::endl;
        string s0;
        cin>>s0;
        std::cout<<"This device id:"<<std::endl;
        string s1;
        cin>>s1;
        std::cout<<"Write a topic"<<std::endl;
        string s3;
        cin>>s3;
        std::cout<<"Write a message"<<std::endl;
        string s2;
        cin>>s2;
        response = client.publishRequest(s0,s3,s2,s1);
        cout<<"Publishing message ..."<<endl;
        std::cout<< response<<std::endl;
        break;
    }
    case 5:
    {
        response = client.showBrokersRequest("Anything");
        std::cout<< response<<std::endl;
        break;
    }
    case 6:
    {
        response = client.showClientsRequest("Anything");
        std::cout<< response<<std::endl;
        break;
    }
    default:
        cout<<"Not a good choice try again"<<endl;
        break;
    }
    //response = client.connectionRequest(adrs);
    //std::cout  << response << std::endl;  
}

int main(int argc, char* argv[]){
    Run();
    return 0;
}