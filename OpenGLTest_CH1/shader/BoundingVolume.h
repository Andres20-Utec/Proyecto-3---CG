

#ifndef E3_OBJETOS_BOUNDINGVOLUME_H
#define E3_OBJETOS_BOUNDINGVOLUME_H
#include "../include/glm/gtx/string_cast.hpp"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"

using namespace glm;
class BoundingVolume {
public:
    virtual bool intersecta(BoundingVolume &bv)=0;
};

class BoundingSphere : public BoundingVolume{
public:
    vec3 centro; float radio;
    BoundingSphere() { centro = vec3(0); radio = 0; }
    BoundingSphere(vec3 _centro, float _radio){
        centro = _centro; radio = _radio;
    }
    bool intersecta(BoundingVolume &bv){}
    bool intersecta(BoundingSphere &bs){
        float distanciaManhattan = abs(bs.centro.x - centro.x) + abs(bs.centro.x - centro.y) + abs(bs.centro.x - centro.z);
        return distanciaManhattan <= bs.radio + radio;
    }
};

#endif //E3_OBJETOS_BOUNDINGVOLUME_H