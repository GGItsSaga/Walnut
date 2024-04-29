#pragma once
#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <iostream>
#include <string>
#include <fstream>

class Account 
{
private:
    std::string username;
    std::string password;
    std::string errorMessage;
    bool isAdmin;

    // Written by Alex 4/10/24
    bool isValidPassword(const std::string& pwd) const 
    {
        return pwd.length() >= 6; // Simple check for password length
    }

    // Written by Danny 4/10/24
    void saveToFile() const 
    {
        std::ofstream outFile("accounts.txt", std::ios::app); // Append mode
        if (!outFile.is_open()) 
        {
            ImGui::OpenPopup("Error Opening File");
            //errorMessage = "Error: Failed to open file for writing.";
            return;
        }

        try 
        {
            outFile << username << " " << password << " " << "no" << std::endl;
            outFile.close(); // Close the file after writing
        }
        catch (const std::exception& e) 
        {
            ImGui::OpenPopup("Error Writing File");
            //errorMessage = "Error while writing to file: " + std::string(e.what());
            outFile.close(); // Make sure to close the file in case of an exception
        }
    }

public:
    Account() : username(""), password(""), isAdmin(false) {} // Constructor, written by David 4/10/24

    std::string createAccount(const std::string& usr, const std::string& pwd)
    {
        // Check if the username already exists. If it's taken, return an error message.
        if (isUsernameTaken(usr))
        {
            return "Username taken";
        }

        username = usr;
        password = pwd;
        getAdminStatus(); // Returns admin status

        saveToFile(); // Save the account data to file
        return "Account created successfully";
    }
    
    // Checks if the username is taken or not. If it is, output an error and ask them to re-enter. Returns true/false for conditions. 
    // Written by Danny, David and Alex 4/10/24
    bool isUsernameTaken(const std::string& usr) const
    {
        std::ifstream inFile("accounts.txt");
        std::string storedUser, storedPass;

        while (inFile >> storedUser >> storedPass)
        {
            if (storedUser == usr)
            {
                inFile.close();
                return true; // Username is already taken
            }
        }

        inFile.close();
        return false; // Username is not taken
    }

    // Checks to see if user is logged in:
    // Written by Alex, Danny and David 4/10/24
    bool login(const std::string& usr, const std::string& pwd) 
    {
        std::ifstream inFile("accounts.txt");
        std::string storedUser, storedPass, adminStatus;

        while (inFile >> storedUser >> storedPass >> adminStatus)
        {
            if (storedUser == usr && storedPass == pwd)
            {
                username = usr;
                isAdmin = (adminStatus == "yes");
                inFile.close();
                return true; // Login successful
            }
        }

        inFile.close();
        return false; // Login failed
    }

    // Returning admin status:
    bool getAdminStatus() const
    {
        return isAdmin;
    }

    // Returns username for log recording: 
    std::string getUsername() const
    {
        return username;
    }
};

#endif // ACCOUNT_H
