#include "OrderCache.h"

void OrderCache::addOrder(const Order& order){
    if(secuityIds.find(order.securityID) == securityIds.end()){
        securityIds[order.securityID] = SecurityId{};
    }
    
}

void OrderCache::cancelOrder(const std::string& orderId){

}

void OrderCache::cancelOrdersForUser(const std::string& uderId){

}

void OrderCache::cancelOrdersAboveQuantity(const std::string& secureId, int quantity){

}

void OrderCache::cancelOrdersAtQuantity(const std::string& secureId, int quantity){

}

void OrderCache::getCurrentQuantity(const std::string& secureId){

}