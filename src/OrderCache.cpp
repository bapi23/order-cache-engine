#include "OrderCache.h"

void OrderCache::addOrder(const Order& order){
    if(securityIds.find(order.securityID) == securityIds.end()){
        securityIds[order.securityID] = SecurityId{};
    }
    securityIds[order.securityID].appendOrder(order);
    orderIdToSecurityId[order.orderID] = order.securityID;
    userIdToOrderIds[order.userID].push_back(order.orderID);
}

void OrderCache::cancelOrder(const std::string& orderId){
    const auto securityID = orderIdToSecurityId[orderId];
    securityIds[securityID].cancelOrderByOrderId(orderId);
}

void OrderCache::cancelOrdersForUser(const std::string& userId){
    for(const auto& orderId: userIdToOrderIds[userId]){
        const auto securityID = orderIdToSecurityId[orderId];
        securityIds[securityID].cancelOrderByOrderId(orderId);
    }
}

void OrderCache::cancelOrdersAboveQuantity(const std::string& securityId, int quantity){

}

void OrderCache::cancelOrdersAtQuantity(const std::string& securityId, int quantity){

}

int OrderCache::getMatchedQuantity(const std::string& securityId) const {
    return securityIds.at(securityId).getMatchedQuantity();
}