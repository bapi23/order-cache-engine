#pragma once

#include <optional>
#include <algorithm>
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

    void cancelOrderByOrderId(const std::string& orderID){

        //Remove buy orders
        for(auto& matcher: matchers){
            matcher.removeBuyOrder(orderID);
        }

        //Remove sell order
        auto matcherIt = std::find_if(matchers.begin(), matchers.end(), 
        [&orderID](const Matcher& matcher){ return matcher.sellOrder.orderID == orderID; });

        if(matcherIt != matchers.end()){
            std::vector<Order> matcherOrders = matcherIt->getAllOrders();
            buyOrderQueue.insert(buyOrderQueue.end(), matcherOrders.begin(), matcherOrders.end());
            matchers.erase(matcherIt);
        }

        feedMatchersWithQueuedOrders();
    }

private:
    std::optional<Order> feedMatchers(const Order& order){
        std::optional<Order> currentOrder = order;
        for(auto& matcher: matchers){
            if(matcher.hasRemainingSellQuantity() && matcher.sellerCompanyName() != order.companyName){
                currentOrder = matcher.addBuyOrder(*currentOrder);
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

    // Looks like we will iterate more often by matchers then performing removal of an item
    // Anyway It could be optimized futher by using different collection but it would be good to know the input size and frequency before.
    std::vector<Matcher> matchers;
    std::vector<Order> buyOrderQueue;
};