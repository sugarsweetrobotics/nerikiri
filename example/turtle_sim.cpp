#include <iostream>
#include "nerikiri/nerikiri.h"
#include "nerikiri/process.h"

using namespace std::literals::string_literals;

using namespace nerikiri;

const std::string jsonStr = R"(
{
    "logger": { "logLevel": "DEBUG" },
    "operations": {
        "load_paths": ["test", "build/test", "../build/test", "../build/example", "../../build/example", "../build/bin/Debug", "../../build/bin/Debug",  "../../../build/lib", "../../build/lib", "../build/lib"],
        "preload": ["joy2turtlevel"],
        "precreate": [
            {
                "name": "joy2turtlevel",
                "instanceName": "joy2turtlevel0.ope"
            }
        ]
    },

    "containers": {
        "load_paths": ["test", "build/test", "../build/test", "../build/example", "../../build/example", "../build/bin/Debug", "../../build/bin/Debug", "../../../build/bin"],
        "preload": {
            "TurtleSim": ["getPose", "kill", "setVelocity", "spawn", "update"],
            "JoyController": ["initialize", "update", "get"]
        },
        "precreate": [
            { 
                "name": "JoyController",
                "instanceName": "JoyController0.ctn",
                "operations": [
                    {
                        "name": "initialize",
                        "instanceName": "initialize0.ope"
                    },
                    {
                        "name": "update",
                        "instanceName": "update0.ope"
                    },
                    {
                        "name": "get",
                        "instanceName": "get0.ope"
                    }
                ]
            },
            {
                "name": "TurtleSim",
                "instanceName": "TurtleSim0.ctn",
                "operations": [
                    {
                        "name": "update",
                        "instanceName": "update0.ope"
                    },
                    {
                        "name": "spawn",
                        "instanceName": "spawn0.ope"
                    },
                    {
                        "name": "kill",
                        "instanceName": "kill0.ope"
                    },
                    {
                        "name": "setVelocity",
                        "instanceName": "setVelocity0.ope"
                    },
                    {
                        "name": "getPose",
                        "instanceName": "getPose0.ope"
                    }
                ]
            }
        ]
    },

    "brokers": {
        "load_paths": ["test", "build/test", "../build/test", "../build/example", "../../build/example", "../build/bin/Debug", "../../build/bin/Debug", "../../../build/bin"],
        "preload": ["HTTPBroker"],
        "precreate": [ 
          {
            "name": "HTTPBroker",
            "instanceName": "HTTPBroker0.brk",
            "host": "0.0.0.0",
            "port": 8080
          }
        ]
    },

    "ecs": {
        "load_paths": ["test", "build/test", "../build/test", "../build/example", "../../build/example", "../build/bin/Debug", "../../build/bin/Debug", "../../../build/bin"],
        "preload": ["TimerEC"],
        "precreate": [
          {
            "name": "TimerEC",
            "instanceName": "TimerEC0.ec",
            "rate": 10.0
          }
        ],
        "bind": {
            "TimerEC0.ec": ["TurtleSim0.ctn:update0.ope", "JoyController0.ctn:update0.ope", "JoyController0.ctn:get0.ope"]
        },
        "start": ["TimerEC0.ec"]
    },


    "connections": [
        { 
            "name": "connection01",
            "type": "event", 
            "broker": {"name": "HTTPBroker"},
            "output": {
                "info": {"instanceName": "JoyController0.ctn:get0.ope"},
                "broker": {"name": "HTTPBroker",
                            "host": "localhost",
                            "port": 8080}
            },
            "input": {
                "info": {"instanceName": "joy2turtlevel0.ope"},
                "broker": {"name": "HTTPBroker",
                    "host": "localhost",
                    "port": 8080},
                "target": {"name": "data"}
            }
        },
        { 
            "name": "connection02",
            "type": "event", 
            "broker": {"name": "HTTPBroker"},
            "output": {
                "info": {"instanceName": "joy2turtlevel0.ope"},
                "broker": {"name": "HTTPBroker",
                            "host": "localhost",
                            "port": 8080}
            },
            "input": {
                "info": {"instanceName": "TurtleSim0.ctn:setVelocity0.ope"},
                "broker": {"name": "HTTPBroker",
                    "host": "localhost",
                    "port": 8080},
                "target": {"name": "data"}
            }
        }
    ],

    "callbacks": [
        {
            "name": "on_started",
            "target": [
                {
                    "name": "JoyController0.ctn:initialize0.ope",
                    "argument": {
                        "arg": ""
                    } 
                }
            ]
        }
    ]
}  
)";

int main(const int argc, const char* argv[]) {
  return nerikiri::Process("turtlesim", jsonStr)
    .start();
}
