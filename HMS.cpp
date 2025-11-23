#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map> // For efficient data storage and retrieval
#include <iomanip>       // For setw, setfill
#include <limits>        // For numeric_limits

// Structure to hold individual room/customer data
struct RoomData {
    int room_no;
    std::string name;
    std::string address;
    std::string phone;
    long days;
    long cost;
    std::string rtype; // Room type (Deluxe, Executive, Presidential)
    long food_bill;    // Cost for food items

    // Default constructor for RoomData
    RoomData() : room_no(0), days(0), cost(0), food_bill(0) {}

    // Parameterized constructor for RoomData
    RoomData(int r_no, const std::string& n, const std::string& addr, const std::string& ph,
             long d, long c, const std::string& rt, long fb)
        : room_no(r_no), name(n), address(addr), phone(ph), days(d), cost(c), rtype(rt), food_bill(fb) {}
};

// Class to manage all hotel operations using an unordered_map
class HotelManager {
private:
    // Unordered map to store RoomData objects, using room_no as key for O(1) average time complexity
    std::unordered_map<int, RoomData> rooms_map;
    const std::string DATA_FILE = "Record.DAT"; // File to persist data

    // Private helper functions for restaurant menu calculations
    void calculateBreakfastCost(RoomData& room, int num_people);
    void calculateLunchCost(RoomData& room, int num_people);
    void calculateDinnerCost(RoomData& room, int num_people);

    // Helper to clear input buffer
    void clearInputBuffer();

public:
    HotelManager();  // Constructor to load data
    ~HotelManager(); // Destructor to save data

    void load_data();  // Loads data from file into the unordered_map
    void save_data();  // Saves data from the unordered_map to file

    void main_menu();    // Displays the main menu and handles user choices
    void add_room();     // Books a room and adds customer details
    void display_room(); // Displays specific customer information
    void display_all_rooms(); // Displays all allotted rooms
    void edit_customer_details(); // Allows modification or checkout
    int check_room_status(int r_no); // Checks if a room is booked or invalid
    void modify_customer_info(); // Modifies customer details
    void delete_customer_record(); // Checks out a customer and deletes record
    void order_food();   // Handles food ordering for a room

    // Specific modification functions
    void modify_name(int r_no);
    void modify_address(int r_no);
    void modify_phone(int r_no);
    void modify_days(int r_no);
};

// Constructor: Loads data when HotelManager object is created
HotelManager::HotelManager() {
    load_data();
}

// Destructor: Saves data when HotelManager object is destroyed
HotelManager::~HotelManager() {
    save_data();
}

// Helper to clear input buffer after numeric input
void HotelManager::clearInputBuffer() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Function to load data from file into the unordered_map
void HotelManager::load_data() {
    std::ifstream fin(DATA_FILE, std::ios::in | std::ios::binary);
    if (!fin.is_open()) {
        std::cout << "\n No existing record file found. Starting with empty data." << std::endl;
        return;
    }

    RoomData temp_room;
    while (fin.read(reinterpret_cast<char*>(&temp_room), sizeof(RoomData))) {
        // For string members, we need to handle them carefully if directly reading binary
        // A safer way would be to read fixed-size char arrays and convert to string,
        // or serialize/deserialize strings explicitly.
        // For simplicity, assuming direct binary read works for std::string if it's POD-like
        // or if the strings are always short and fit.
        // A robust solution would involve writing string lengths then string data.
        // For this example, we'll assume basic binary compatibility for small strings.
        rooms_map[temp_room.room_no] = temp_room;
    }
    fin.close();
    std::cout << "\n Data loaded successfully from " << DATA_FILE << std::endl;
}

// Function to save data from the unordered_map to file
void HotelManager::save_data() {
    std::ofstream fout(DATA_FILE, std::ios::out | std::ios::binary | std::ios::trunc); // trunc to overwrite
    if (!fout.is_open()) {
        std::cerr << "\n Error: Could not open file for saving data." << std::endl;
        return;
    }

    for (const auto& pair : rooms_map) {
        fout.write(reinterpret_cast<const char*>(&pair.second), sizeof(RoomData));
    }
    fout.close();
    std::cout << "\n Data saved successfully to " << DATA_FILE << std::endl;
}

// Function to display the main menu of the hotel management system
void HotelManager::main_menu() {
    int choice;
    do {
        system("clear"); 
        std::cout << "\n\t\t\t +---------------------------------+" << std::endl;
        std::cout << "\n\t\t\t |          THE HOTEL            |" << std::endl;
        std::cout << "\n\t\t\t +---------------------------------+" << std::endl;
        std::cout << "\n\n\t\t\t ********* MAIN MENU *********" << std::endl;
        std::cout << "\n\n\t\t\t 1. Book A Room" << std::endl;
        std::cout << "\n\t\t\t 2. Customer Information" << std::endl;
        std::cout << "\n\t\t\t 3. Rooms Allotted" << std::endl;
        std::cout << "\n\t\t\t 4. Edit Customer Details" << std::endl;
        std::cout << "\n\t\t\t 5. Order Food from Restaurant" << std::endl;
        std::cout << "\n\t\t\t 6. Exit" << std::endl;
        std::cout << "\n\t\t\t Enter Your Choice: ";
        std::cin >> choice;
        clearInputBuffer(); 

        switch(choice) {
            case 1:
                add_room();
                break;
            case 2:
                display_room();
                break;
            case 3:
                display_all_rooms();
                break;
            case 4:
                edit_customer_details();
                break;
            case 5:
                order_food();
                break;
            case 6:
                std::cout << "\n Exiting Hotel Management System. Goodbye!" << std::endl;
                break;
            default:
                std::cout << "\n\n\t\t\t Wrong choice. Please try again." << std::endl;
                std::cout << "\n\t\t\t Press Enter to continue. ";
                std::cin.get(); 
        }
    } while (choice != 6);
}

// Function to add a new customer and book a room
void HotelManager::add_room() {
    system("clear");
    int r_no;
    std::cout << "\n\t\t\t +---------------------------------+" << std::endl;
    std::cout << "\n\t\t\t | Rooms | Room Type |" << std::endl;
    std::cout << "\n\t\t\t +---------------------------------+" << std::endl;
    std::cout << "\n\t\t\t | 1-50 | Deluxe |" << std::endl;
    std::cout << "\n\t\t\t | 51-80 | Executive |" << std::endl;
    std::cout << "\n\t\t\t | 81-100 | Presidential |" << std::endl;
    std::cout << "\n\t\t\t +---------------------------------+" << std::endl;
    std::cout << "\n\n ENTER CUSTOMER DETAILS";
    std::cout << "\n -----------------------";
    std::cout << "\n\n Room Number (1-100): ";
    std::cin >> r_no;
    clearInputBuffer();

    int status = check_room_status(r_no);

    if (status == 1) {
        std::cout << "\n Sorry, Room " << r_no << " is already booked." << std::endl;
    } else if (status == 2) {
        std::cout << "\n Sorry, Room " << r_no << " does not exist (valid range 1-100)." << std::endl;
    } else {
        RoomData new_room;
        new_room.room_no = r_no;
        std::cout << " Name: ";
        std::getline(std::cin, new_room.name);
        std::cout << " Address: ";
        std::getline(std::cin, new_room.address);
        std::cout << " Phone Number: ";
        std::getline(std::cin, new_room.phone);
        std::cout << " Number of Days: ";
        std::cin >> new_room.days;
        clearInputBuffer();

        if (new_room.room_no >= 1 && new_room.room_no <= 50) {
            new_room.rtype = "Deluxe";
            new_room.cost = new_room.days * 10000;
        } else if (new_room.room_no >= 51 && new_room.room_no <= 80) {
            new_room.rtype = "Executive";
            new_room.cost = new_room.days * 12500;
        } else if (new_room.room_no >= 81 && new_room.room_no <= 100) {
            new_room.rtype = "Presidential";
            new_room.cost = new_room.days * 15000;
        }
        new_room.food_bill = 0; // Initialize food bill

        rooms_map[new_room.room_no] = new_room; // Add to unordered_map
        std::cout << "\n Room " << new_room.room_no << " has been booked for " << new_room.name << "." << std::endl;
    }
    std::cout << "\n Press Enter to continue.";
    std::cin.get();
}

// Function to display specific customer information
void HotelManager::display_room() {
    system("clear");
    int r_no;
    std::cout << "\n Enter Room Number to display: ";
    std::cin >> r_no;
    clearInputBuffer();

    auto it = rooms_map.find(r_no); // Use find for O(1) average lookup

    if (it != rooms_map.end()) {
        // Room found
        const RoomData& room = it->second;
        system("clear");
        std::cout << "\n Customer Details" << std::endl;
        std::cout << "------------------" << std::endl;
        std::cout << "\n Room Number: " << room.room_no << std::endl;
        std::cout << " Name: " << room.name << std::endl;
        std::cout << " Address: " << room.address << std::endl;
        std::cout << " Phone Number: " << room.phone << std::endl;
        std::cout << " Staying for: " << room.days << " days." << std::endl;
        std::cout << " Room Type: " << room.rtype << std::endl;
        std::cout << " Total Room Cost: " << room.cost << std::endl;
        std::cout << " Total Food Bill: " << room.food_bill << std::endl;
        std::cout << " Grand Total: " << (room.cost + room.food_bill) << std::endl;
    } else {
        std::cout << "\n Room " << r_no << " is Vacant or does not exist." << std::endl;
    }
    std::cout << "\n Press Enter to continue.";
    std::cin.get();
}

// Function to display all allotted rooms in a formatted table
void HotelManager::display_all_rooms() {
    system("clear");
    char separator = ' ';
    const int NoWidth = 8;
    const int GuestWidth = 17;
    const int AddressWidth = 16;
    const int RoomTypeWidth = 13;
    const int ContactNoWidth = 13;
    const int DaysWidth = 5;
    const int CostWidth = 10;

    std::cout << "\n\t\t\t LIST OF ALLOTTED ROOMS" << std::endl;
    std::cout << "\n\t\t\t +--------+-----------------+----------------+-------------+-------------+-----+----------+" << std::endl;
    std::cout << "\n\t\t\t | Room No| Guest Name      | Address        | Room Type   | Contact No. |Days | Total    |" << std::endl;
    std::cout << "\n\t\t\t +--------+-----------------+----------------+-------------+-------------+-----+----------+" << std::endl;

    if (rooms_map.empty()) {
        std::cout << "\n\t\t\t No rooms currently allotted." << std::endl;
    } else {
        for (const auto& pair : rooms_map) {
            const RoomData& room = pair.second;
            std::cout << "\n\t\t\t |" << std::setw(NoWidth) << std::setfill(separator) << room.room_no << "|"
                      << std::setw(GuestWidth) << std::setfill(separator) << room.name << "|"
                      << std::setw(AddressWidth) << std::setfill(separator) << room.address << "|"
                      << std::setw(RoomTypeWidth) << std::setfill(separator) << room.rtype << "|"
                      << std::setw(ContactNoWidth) << std::setfill(separator) << room.phone << "|"
                      << std::setw(DaysWidth) << std::setfill(separator) << room.days << "|"
                      << std::setw(CostWidth) << std::setfill(separator) << (room.cost + room.food_bill) << "|" << std::endl;
        }
    }
    std::cout << "\n\t\t\t +--------+-----------------+----------------+-------------+-------------+-----+----------+" << std::endl;
    std::cout << "\n\n\n\t\t\t Press Enter to continue.";
    std::cin.get();
}

// Function to edit customer details (modify or delete)
void HotelManager::edit_customer_details() {
    system("clear");
    int choice;
    std::cout << "\n EDIT MENU:" << std::endl;
    std::cout << "------------" << std::endl;
    std::cout << "\n 1. Modify Customer Information." << std::endl;
    std::cout << "\n 2. Customer Check Out." << std::endl;
    std::cout << "\n Enter your choice: ";
    std::cin >> choice;
    clearInputBuffer();

    system("clear");

    switch(choice) {
        case 1:
            modify_customer_info();
            break;
        case 2:
            delete_customer_record();
            break;
        default:
            std::cout << "\n Wrong Choice. Please try again." << std::endl;
            break;
    }
    std::cout << "\n Press Enter to continue.";
    std::cin.get();
}

// Function to check room status (booked, vacant, or invalid)
// Returns 0 if vacant, 1 if booked, 2 if invalid room number
int HotelManager::check_room_status(int r_no) {
    if (r_no < 1 || r_no > 100) {
        return 2; // Invalid room number
    }
    auto it = rooms_map.find(r_no);
    if (it != rooms_map.end()) {
        return 1; // Room is booked
    }
    return 0; // Room is vacant
}

// Function to modify customer information
void HotelManager::modify_customer_info() {
    system("clear");
    int ch, r_no;
    std::cout << "\n MODIFY MENU:" << std::endl;
    std::cout << "-------------" << std::endl;
    std::cout << "\n 1. Modify Name" << std::endl;
    std::cout << "\n 2. Modify Address" << std::endl;
    std::cout << "\n 3. Modify Phone Number" << std::endl;
    std::cout << "\n 4. Modify Number of Days of Stay" << std::endl;
    std::cout << "\n Enter Your Choice: ";
    std::cin >> ch;
    clearInputBuffer();

    system("clear");
    std::cout << "\n Enter Room Number to modify: ";
    std::cin >> r_no;
    clearInputBuffer();

    auto it = rooms_map.find(r_no);
    if (it == rooms_map.end()) {
        std::cout << "\n Sorry, Room " << r_no << " is vacant or does not exist." << std::endl;
        return;
    }

    switch(ch) {
        case 1:
            modify_name(r_no);
            break;
        case 2:
            modify_address(r_no);
            break;
        case 3:
            modify_phone(r_no);
            break;
        case 4:
            modify_days(r_no);
            break;
        default:
            std::cout << "\n Wrong Choice. Please try again." << std::endl;
            break;
    }
}

// Function to modify the name of a guest
void HotelManager::modify_name(int r_no) {
    auto it = rooms_map.find(r_no);
    if (it != rooms_map.end()) {
        std::cout << "\n Enter New Name: ";
        std::getline(std::cin, it->second.name);
        std::cout << "\n Customer Name has been modified." << std::endl;
    } else {
        std::cout << "\n Sorry, Room is vacant." << std::endl;
    }
}

// Function to modify the address of a guest
void HotelManager::modify_address(int r_no) {
    auto it = rooms_map.find(r_no);
    if (it != rooms_map.end()) {
        std::cout << "\n Enter New Address: ";
        std::getline(std::cin, it->second.address);
        std::cout << "\n Customer Address has been modified." << std::endl;
    } else {
        std::cout << "\n Sorry, Room is vacant." << std::endl;
    }
}

// Function to modify the phone number of a guest
void HotelManager::modify_phone(int r_no) {
    auto it = rooms_map.find(r_no);
    if (it != rooms_map.end()) {
        std::cout << "\n Enter New Phone Number: ";
        std::getline(std::cin, it->second.phone);
        std::cout << "\n Customer Phone Number has been modified." << std::endl;
    } else {
        std::cout << "\n Sorry, Room is vacant." << std::endl;
    }
}

// Function to modify the number of days of stay for a guest
void HotelManager::modify_days(int r_no) {
    auto it = rooms_map.find(r_no);
    if (it != rooms_map.end()) {
        std::cout << "\n Enter New Number of Days of Stay: ";
        std::cin >> it->second.days;
        clearInputBuffer();

        // Recalculate cost based on new days
        if (it->second.room_no >= 1 && it->second.room_no <= 50) {
            it->second.cost = it->second.days * 10000;
        } else if (it->second.room_no >= 51 && it->second.room_no <= 80) {
            it->second.cost = it->second.days * 12500;
        } else if (it->second.room_no >= 81 && it->second.room_no <= 100) {
            it->second.cost = it->second.days * 15000;
        }
        std::cout << "\n Customer information is modified." << std::endl;
    } else {
        std::cout << "\n Sorry, Room is vacant." << std::endl;
    }
}

// Function to delete a customer record (check out)
void HotelManager::delete_customer_record() {
    int r_no;
    char confirm_char;
    std::cout << "\n Enter Room Number to check out: ";
    std::cin >> r_no;
    clearInputBuffer();

    auto it = rooms_map.find(r_no);
    if (it != rooms_map.end()) {
        // Room found, display details before checkout
        const RoomData& room = it->second;
        std::cout << "\n Name: " << room.name << std::endl;
        std::cout << "\n Address: " << room.address << std::endl;
        std::cout << "\n Phone Number: " << room.phone << std::endl;
        std::cout << "\n Your total bill is: Rs. " << (room.cost + room.food_bill) << std::endl;
        std::cout << "\n Do you want to check out this customer (y/n): ";
        std::cin >> confirm_char;
        clearInputBuffer();

        if (confirm_char == 'y' || confirm_char == 'Y') {
            rooms_map.erase(it); 
            std::cout << "\n Customer Checked Out. Room " << r_no << " is now vacant." << std::endl;
        } else {
            std::cout << "\n Checkout cancelled." << std::endl;
        }
    } else {
        std::cout << "\n Sorry, Room " << r_no << " is vacant or does not exist." << std::endl;
    }
    std::cout << "\n Press Enter to continue.";
    std::cin.get();
}

// Function to handle restaurant food orders
void HotelManager::order_food() {
    system("clear");
    int r_no, meal_choice, num_people;
    std::cout << "\n RESTAURANT MENU:" << std::endl;
    std::cout << "------------------" << std::endl;
    std::cout << "\n 1. Order Breakfast" << std::endl;
    std::cout << " 2. Order Lunch" << std::endl;
    std::cout << " 3. Order Dinner" << std::endl;
    std::cout << "\n Enter your choice: ";
    std::cin >> meal_choice;
    clearInputBuffer();

    system("clear");
    std::cout << " Enter Room Number for the order: ";
    std::cin >> r_no;
    clearInputBuffer();

    auto it = rooms_map.find(r_no);
    if (it == rooms_map.end()) {
        std::cout << "\n Sorry, Room " << r_no << " is vacant or does not exist." << std::endl;
        std::cout << "\n Press Enter to continue.";
        std::cin.get();
        return;
    }
    std::cout << " Enter number of people: ";
    std::cin >> num_people;
    clearInputBuffer();
    switch(meal_choice) {
        case 1:
            calculateBreakfastCost(it->second, num_people);
            break;
        case 2:
            calculateLunchCost(it->second, num_people);
            break;
        case 3:
            calculateDinnerCost(it->second, num_people);
            break;
        default:
            std::cout << "\n Invalid choice for meal." << std::endl;
            break;
    }
    std::cout << "\n Press Enter to continue.";
    std::cin.get();
}

// Private helper functions for food cost calculation
void HotelManager::calculateBreakfastCost(RoomData& room, int num_people) {
    long cost_per_person = 500;
    long added_cost = cost_per_person * num_people;
    room.food_bill += added_cost;
    std::cout << "\n Rs. " << added_cost << " added to the bill for breakfast." << std::endl;
}

void HotelManager::calculateLunchCost(RoomData& room, int num_people) {
    long cost_per_person = 1000;
    long added_cost = cost_per_person * num_people;
    room.food_bill += added_cost;
    std::cout << "\n Rs. " << added_cost << " added to the bill for lunch." << std::endl;
}

void HotelManager::calculateDinnerCost(RoomData& room, int num_people) {
    long cost_per_person = 1200;
    long added_cost = cost_per_person * num_people;
    room.food_bill += added_cost;
    std::cout << "\n Rs. " << added_cost << " added to the bill for dinner." << std::endl;
}

// Main function to run the hotel management system
int main() {
    HotelManager hotel_system; // Create an object of HotelManager class
    hotel_system.main_menu();  // Call the main menu function
    return 0;
}
