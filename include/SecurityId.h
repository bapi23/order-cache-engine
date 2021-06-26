#pragma once

#include <optional>
#include "Matcher.h"


class SecurityId{
public:
    int getMatchedQuantity() const {
        int quantity = 0;
        for(const auto& matcher: matchers){
            quantity+=matcher.getMatchedQuantity();
        }

        return quantity;
    }

    void appendOrder(const Order& order){
        if(order.side == "Sell"){
            matchers.emplace_back(order);
            feedMatchersWithQueuedOrders();
        } else {
            auto remainingOrder = feedMatchers(order);
            //if no one matcher handled entire buy request:
            if(remainingOrder){
                buyOrderQueue.push_back(*remainingOrder);
            }
        }
    }

    void cancelOrderByuserID(const std::string& userID){
        for(auto& matcher: matchers){
            matcher.removeBuyOrderByUserID(userID);
        }
        feedMatchersWithQueuedOrders();
    }

    void cancelOrderByOrderId(const std::string& orderId){
        for(auto& matcher: matchers){
            matcher.removeBuyOrder(orderId);
        }
        feedMatchersWithQueuedOrders();
    }

    private:
    std::optional<Order> feedMatchers(const Order& order){
        std::optional<Order> currentOrder = order;
        for(auto& matcher: matchers){
            if(matcher.hasRemainingSellQuantity() && matcher.sellerCompanyName() != order.companyName){
                matcher.addBuyOrder(*currentOrder);
                currentOrder = matcher.getRemainingBuyOrder();
                if(!currentOrder){
                    return currentOrder;
                }
            }
        }
        return currentOrder;
    }

    void feedMatchersWithQueuedOrders(){
        //check if we can handle now existing buy orders:
        std::vector<Order> resultQueue;
        for(const auto& order: buyOrderQueue){
            auto remainingOrder = feedMatchers(order);
            if(remainingOrder){
                resultQueue.push_back(*remainingOrder);
            }
        }
        buyOrderQueue = resultQueue;
    }

    std::vector<Matcher> matchers;
    std::vector<Order> buyOrderQueue;
};