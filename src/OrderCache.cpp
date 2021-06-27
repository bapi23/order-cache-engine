#include "OrderCache.h"

void OrderCache::addOrder(const Order& order){
    if(securityIds.find(order.securityID) == securityIds.end()){
        securityIds[order.securityID] = SecurityId{};
    }
    securityIds[order.securityID].appendOrder(order);
    orderIdToSecurityId[order.orderID] = order.securityID;
    userIdToOrderIds[order.userID].push_back(order.orderID);
}

void OrderCache::cancelOrder(const std::string& orderID){
    const auto securityID = orderIdToSecurityId[orderID];
    securityIds[securityID].cancelOrderByOrderId(orderID);
    orderIdToSecurityId.erase(orderID);
}

void OrderCache::cancelOrdersForUser(const std::string& userID){
    for(const auto& orderID: userIdToOrderIds[userID]){
        cancelOrder(orderID);
    }
    userIdToOrderIds.erase(userID);
}

void OrderCache::cancelOrdersAboveQuantity(const std::string& securityId, int quantity){
}

void OrderCache::cancelOrdersAtQuantity(const std::string& securityId, int quantity){

}

int OrderCache::getMatchedQuantity(const std::string& securityId) const {
    return securityIds.at(securityId).getMatchedQuantity();
}