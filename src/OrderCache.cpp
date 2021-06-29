#include "OrderCache.h"

void OrderCache::addOrder(Order order){
    if(securityIds.find(order.securityID) == securityIds.end()){
        securityIds[order.securityID] = SecurityId{};
    }
    orderIdToSecurityId[order.orderID] = order.securityID;
    userIdToOrderIds[order.userID].push_back(order.orderID);
    
    securityIds[order.securityID].appendOrder(std::move(order));
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

void OrderCache::cancelOrdersAboveQuantity(const std::string& securityID, int quantity){
    securityIds.at(securityID).cancelOrderAboveQuantity(quantity);
}

void OrderCache::cancelOrdersAtQuantity(const std::string& securityID, int quantity){
    securityIds.at(securityID).cancelOrderAtQuantity(quantity);
}

int OrderCache::getMatchedQuantity(const std::string& securityId) const {
    return securityIds.at(securityId).getMatchedQuantity();
}