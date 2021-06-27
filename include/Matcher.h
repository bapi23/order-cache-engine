#pragma once

#include <cassert>
#include <iostream>

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

    std::optional<Order> addBuyOrder(const Order& order){
        assert(sellOrder.companyName != order.companyName);

        int maxMatched = std::min(order.quantity, sellOrder.remainingQuantity);
        sellOrder.remainingQuantity -= maxMatched;
        orders[order.orderID] = order;
        orders[order.orderID].quantity -= maxMatched;
        std::cout << "Sell" << sellOrder.orderID << ": before adding Buy:" << order.quantity << std::endl;
        std::cout << "Sell" << sellOrder.orderID << ": after adding Buy:" << orders[order.orderID].quantity << std::endl;

        if(orders[order.orderID].quantity > 0){
            Order remainingOrder = orders[order.orderID];
            remainingOrder.quantity = orders[order.orderID].quantity;
            orders[order.orderID].quantity = maxMatched;
            return remainingOrder;
        }
        orders[order.orderID].quantity = maxMatched;

        return std::optional<Order>();
    }

    void removeBuyOrder(const std::string& orderID){
        auto foundIt = orders.find(orderID);
        if(foundIt != orders.end()){
            orders.erase(orderID);
            const auto quantity_to_add = orders[orderID].quantity;
            std::cout << "Adding quantity" << quantity_to_add << std::endl;
            sellOrder.remainingQuantity += quantity_to_add;
        }
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
        return sellOrder.quantity - sellOrder.remainingQuantity;
    }

    bool hasRemainingSellQuantity(){
        return sellOrder.remainingQuantity > 0;
    }

    std::string sellerCompanyName(){
        return sellOrder.companyName;
    }

    std::vector<Order> getAllOrders(){
        std::vector<Order> output;
        for(const auto& order: orders){
            output.push_back(order.second);
            std::cout << "Removing order " << order.second.quantity << std::endl;
        }
        return output;
    }

    Order sellOrder;
    //Unordered map because order doesn't matter inside matcher.
    std::unordered_map<std::string, Order> orders;
    
};