#include <juiz/juiz.h>
#include <juiz/container.h>

#include "DemoBehaviour.h"

using namespace juiz;

namespace {
    auto daruma_mesh = Value{
            {"name", "modelData"},
            {"links", Value::valueList(
                Value{
                    {"name", "root"},
                    {"visual", {
                        {"geometry", {
                            {"box", {
                                {"size", Value::valueList(0.5, 0.5, 0.5)}
                            }}
                        }},
                        {"material", {
                            {"script", "red"}
                        }}
                    }},
                    {"pose", Value::valueList(0., 0., 0., 0., 0., 0.)}
                },
                Value{
                    {"name", "neck"},
                    {"visual", {
                        {"geometry", {
                            {"box", {
                                {"size", Value::valueList(0.3, 0.3, 0.3)}
                            }}
                        }},
                        {"material", {
                            {"script", "red"}
                        }}
                    }},
                    {"pose", Value::valueList(0., 0., 0.5, 0., 0., 0.)}
                }
            )}
        };
}

extern "C" {

    JUIZ_OPERATION  void* createDemoBehaviour() {
        return containerFactory<DemoBehaviour>(Value{{"mesh", daruma_mesh}});
    }

}

