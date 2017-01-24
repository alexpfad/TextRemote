
/*
void addAuthorizedNumber(int phoneNumber) {
    // this adds a phone number to the list of numbers approved to operate vehicle via SMS
    // Note: users can only be added to spots 3 and 4 on the Approved Numbers list
    
    if ( 0 == ApprovedNumbers[2] ) {          // checks to see if slot 3 is empty
        ApprovedNumbers[2] = phoneNumber;   // if so, the new number is added
    }
    else if ( 0 == ApprovedNumbers[3] ) {    // checks to see if slot 4 is empty
        ApprovedNumbers[3] = phoneNumber;   // if so, the new number is added
    }
    else {     // if we've gotten this far, there are no empty slots!
        sendText("Error: The list of approved numbers is already full!");
        return;
    }

    sendText("The number has been added to the approved list!");
    return;
}


void removeAuthorizedNumber(int phoneNumber) {
    // this removes a phone number from the list of approved numbers
    // my numbers cannot be removed!

    if ( (ALEXCELL == phoneNumber) || (ALEXGOOGLE == phoneNumber ) ) {  // if an attempt is made to remove my number from the list...
        sendText("Attempt to remove number failed. You cannot remove Alex's phone numbers from the list.");
    }
    else {
        if ( phoneNumber == ApprovedNumbers[2] ) {   // checks if the number to remove is in spot 3
            ApprovedNumbers[2] = 0;                  // if so, spot 3 is wiped
        }
        else if ( phoneNumber == ApprovedNumbers[3] ) {   // checks if the number to remove is in spot 4
            ApprovedNumbers[3] = 0;                       // if so, spot 4 is wiped
        }
        else { // if we got this far, we know that the number to remove was not on the list of approved numbers
            sendText("Attempt to remove number failed. The number was not on the approved list.");
            return;    
        }
        sendText("The number has been successfully removed from the list of approved numbers");
    }
    return;
}
*/

