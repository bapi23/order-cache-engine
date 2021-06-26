#pragma once

#include <cassert>

#include <vector>
#include <queue>
#include <memory>
#include <optional>
#include "Order.hpp"


class Matcher{

public:
    Matcher(const Order& sellOrder):
        sellOrder(sellOrder)
    {
    }

    void addBuyOrder(const Order& order){
        assert(sellOrder.companyName != order.companyName);

        int maxMatched = std::min(order.remainingQuantity, sellOrder.remainingQuantity);
        sellOrder.remainingQuantity -= maxMatched;
        orders[order.orderID] = order;
        orders[order.orderID].remainingQuantity -=maxMatched;
    }

    void removeBuyOrder(const std::string& order_id){

        orders.erase(order_id);
        const auto quantity_to_add = orders[order_id].quantity - orders[order_id].remainingQuantity;
        sellOrder.remainingQuantity += quantity_to_add;
    }

    void removeBuyOrderByUserID(const std::string& userID){
        std::vector<std::string> idsToRemove;
        for(const auto& order: orders){
            if(order.second.userID == userID){
                idsToRemove.push_back(order.second.orderID);
            }
        }

        for(const auto& orderId: idsToRemove){
            removeBuyOrder(orderId);
        }
    }

    int getMatchedQuantity() const {
        sellOrder.quantity - sellOrder.remainingQuantity;
    }

    bool hasRemainingSellQuantity(){
        return sellOrder.remainingQuantity > 0;
    }

    std::string sellerCompanyName(){
        return sellOrder.companyName;
    }

    std::optional<Order> getRemainingBuyOrder(){
        for(auto const& order: orders){
            if(order.second.remainingQuantity > 0){
                Order remainingOrder = order.second;
                remainingOrder.quantity = remainingOrder.remainingQuantity;
                remainingOrder.remainingQuantity = remainingOrder.quantity;
                return remainingOrder;
            }
        }
        return std::optional<Order>();
    }

    Order sellOrder;
    std::unordered_map<std::string, Order> orders;
    
};