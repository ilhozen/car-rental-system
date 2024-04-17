#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <forward_list>
using namespace std;

class Car {
private:
	string type;
	string brand;
	string model;
	string year;
	bool isRented;
public:
	Car(string t, string b, string m, string y) :type(t), brand(b), model(m), year(y), isRented(false) {}

	void settype(string t) { type = t; }
	void setbrand(string b) { brand = b; }
	void setmodel(string m) { model = m; }
	void setyear(string y) { year = y; }
	void setRented(bool tf) { isRented = tf; }

	string gettype() { return type; }
	string getbrand() { return brand; }
	string getmodel() { return model; }
	string getyear() { return year; }
	bool getRented() { return isRented; }

	friend ostream& operator<<(ostream& output, const Car& car)
	{
		output << "Type: " << car.type << ", Brand: " << car.brand << ", Model: " << car.model << ", Year: " << car.year << ", Rented: ";
		if (car.isRented) {
			output << "Yes" << endl;
		}
		else {
			output << "No" << endl;
		}
		return output;
	}
};

class Customer {
private:
	string name;
	string surname;
	string contact;
	forward_list<string> history;
	forward_list<Car> currentCars;
public:
	Customer(string n, string sn, string c) :name(n), surname(sn), contact(c) {}

	void setname(string n) { name = n; }
	void setsurname(string sn) { surname = sn; }
	void setcontact(string c) { contact = c; }

	string getname() { return name; }
	string getsurname() { return surname; }
	string getcontact() { return contact; }

	friend ostream& operator<<(ostream& output, const Customer& customer)
	{
		output << "Name: " << customer.name
			<< "\nSurname: " << customer.surname
			<< "\nContact: " << customer.contact << endl;
		return output;
	}


	void addToCurrentCars(Car* c) {
		currentCars.emplace_front(*c);
	}

	void removeFromCurrentCars(Car* car) {
		currentCars.remove_if(
			[car](Car& c) {
				return (car->getbrand() == c.getbrand() && car->getmodel() == c.getmodel());
			}
		);
	}

	bool checkCurrentCars(Car* car) {
		for (Car& c : currentCars) {
			if (car->getbrand() == c.getbrand() && car->getmodel() == c.getmodel()) {
				return true;
			}
		}
		return false;
	}

	void modifyHistory(Car* c) {
		if (c->getRented()) {
			history.emplace_front("Rented " + c->gettype() + ": " + c->getyear() + " " + c->getbrand() + " " + c->getmodel());
		}
		else {
			history.emplace_front("Returned " + c->gettype() + ": " + c->getyear() + " " + c->getbrand() + " " + c->getmodel());
		}
	}

	void printHistory() {
		if (history.empty()) {
			cout << "This account has no rental history.\n" << endl;
		}
		else {
			cout << "History:" << endl;
			for (string& s : history) {
				cout << s << endl;
			}
			cout << endl;
		}
	}
};

void printInfo(forward_list<Customer>* cuslist, Customer* customer) {
	for (Customer& cus : *cuslist) {
		if (customer->getname() == cus.getname() && customer->getsurname() == cus.getsurname()) {
			cout << cus;
			cus.printHistory();
		}
	}
}

void displayCars(forward_list<Car>* clist) {
	cout << "\n\n                    Cars of EliteDrive Rental Service\n" << endl;
	for (Car& c : *clist) {
		cout << c;
	}
	cout << endl;
}

bool checkValidCar(forward_list<Car>* clist, Car* car) {
	for (Car& c : *clist) {
		if (car->getbrand() == c.getbrand() && car->getmodel() == c.getmodel()) {
			return true;
		}
	}
	return false;
}

bool checkAvailable(forward_list<Car>* clist, Car* car) {
	for (Car& c : *clist) {
		if (car->gettype() == c.gettype() && car->getbrand() == c.getbrand() && car->getmodel() == c.getmodel() && c.getRented() == false) {
			return true;
		}
	}
	return false;
}

bool checkValidCustomer(forward_list<Customer>* cuslist, Customer* customer) {
	for (Customer& cus : *cuslist) {
		if (customer->getname() == cus.getname() && customer->getsurname() == cus.getsurname()) {
			return true;
		}
	}
	return false;
}

void rentCar(forward_list<Car>* clist, forward_list<Customer>* cuslist, Car* car, Customer* customer) {
	for (Customer& cus : *cuslist) {
		if (!(customer->getname() == cus.getname() && customer->getsurname() == cus.getsurname())) {
			continue;
		}
		for (Car& c : *clist) {
			if (car->gettype() == c.gettype() && car->getbrand() == c.getbrand() && car->getmodel() == c.getmodel() && c.getRented() == false) {
				c.setRented(true);
				cus.addToCurrentCars(&c);
				cus.modifyHistory(&c);
				break;
			}
		}
	}
}

bool checkDuplicateAccount(forward_list<Customer>* cuslist, Customer* customer) {
	for (Customer& cus : *cuslist) {
		if (customer->getname() == cus.getname() && customer->getsurname() == cus.getsurname() && customer->getcontact() == cus.getcontact()) {
			return true;
		}
	}
	return false;
}

void returnCar(forward_list<Car>* clist, forward_list<Customer>* cuslist, Car* car, Customer* customer) {
	for (Customer& cus : *cuslist) {
		if (cus.checkCurrentCars(car) && customer->getname() == cus.getname() && customer->getsurname() == cus.getsurname()) {
			cus.removeFromCurrentCars(car);
			for (Car& c : *clist) {
				if (car->getbrand() == c.getbrand() && car->getmodel() == c.getmodel()) {
					c.setRented(false);
					cus.modifyHistory(&c);
					break;
				}
			}

		}
	}
}

int main() {
	string line, type, brand, model, year, name, surname, contact;
	bool skip = true;
	int choice;
	forward_list <Car> carList;
	forward_list <Customer> customerList;

	ifstream carFile("cars.txt");

	if (!carFile.is_open()) {
		cout << "Unable to open file." << endl;
		return 0;
	}


	while (getline(carFile, line)) {
		stringstream carstream(line);
		if (!skip) {
			if (getline(carstream, type, ',') && getline(carstream, brand, ',') && getline(carstream, model, ',') && getline(carstream, year, ',')) {
				carList.emplace_front(type, brand, model, year);
			}
		}
		skip = false;
	}

	do {
		cout << "------------------------------------------\n"
			<< "|      EliteDrive Car Rental System      |\n"
			<< "|----------------------------------------|\n"
			<< "|1. Display Cars                         |\n"
			<< "|----------------------------------------|\n"
			<< "|2. Rent a Car                           |\n"
			<< "|----------------------------------------|\n"
			<< "|3. Return a Car                         |\n"
			<< "|----------------------------------------|\n"
			<< "|4. Display Customer Information         |\n"
			<< "|----------------------------------------|\n"
			<< "|5. Add New Customer                     |\n"
			<< "|----------------------------------------|\n"
			<< "|6. View All Accounts                    |\n"
			<< "|----------------------------------------|\n"
			<< "|7. Exit                                 |\n"
			<< "------------------------------------------\n"
			<< "Enter your choice: ";
		cin >> choice;
		cin.ignore();
		switch (choice) {
		case 1:
		{
			displayCars(&carList);
			break;
		}
		case 2:
		{
			cout << "\nRent a Car" << endl;
			cout << "\nPlease enter the type of the car for the rental: ";
			cin >> type;
			cout << "Please enter the brand of the car for the rental: ";
			cin >> brand;
			cout << "Please enter the model of the car for the rental: ";
			cin >> model;

			Car car(type, brand, model, "");
			if (!checkAvailable(&carList, &car)) {
				cout << "A car with the given parameters does not exist/ is not currently available.\n" << endl;
				skip = true;
			}
			if (!skip) {
				cout << "\nPlease enter the name of the Customer: ";
				cin >> name;
				cout << "Please enter the surname of the customer: ";
				cin >> surname;

				Customer customer(name, surname, "");
				if (!checkValidCustomer(&customerList, &customer)) {
					cout << "No customer account with that name was found.\n"
						<< "Please create an account before trying again.\n" << endl;
				}
				else {
					rentCar(&carList, &customerList, &car, &customer);
					cout << "Car has been successfully rented.\n" << endl;
				}
			}
			skip = false;
			break;
		}
		case 3:
		{
			cout << "\nReturn a Car" << endl;
			cout << "Please enter the brand of the car to be returned: ";
			cin >> brand;
			cout << "Please enter the model of the car to be returned: ";
			cin >> model;

			Car car("", brand, model, "");

			if (!checkValidCar(&carList, &car)) {
				cout << "A car with the given parameters does not exist.\n" << endl;
				skip = true;
			}
			if (!skip) {
				cout << "\nPlease enter the name of the customer: ";
				cin >> name;
				cout << "Please enter the surname of the customer: ";
				cin >> surname;

				Customer customer(name, surname, "");
				if (!checkValidCustomer(&customerList, &customer)) {
					cout << "No customer account with that name was found.\n"
						<< "Please create an account and rent a car before trying again.\n" << endl;
				}
				else {
					returnCar(&carList, &customerList, &car, &customer);
					cout << "Car has been returned successfully.\n" << endl;
				}
			}
			skip = false;
			break;
		}
		case 4:
		{
			cout << "Display Customer Info" << endl;
			cout << "\nPlease enter the name of the customer: ";
			cin >> name;
			cout << "Please enter the surname of the customer: ";
			cin >> surname;

			Customer customer(name, surname, "");
			if (!checkValidCustomer(&customerList, &customer)) {
				cout << "No customer account with that name was found.\n"
					<< "Please enter a valid name or create an account before trying again.\n" << endl;
			}
			else {
				cout << "\nCustomer Information:" << endl;
				printInfo(&customerList, &customer);
			}
		}
		break;
		case 5:
		{
			cout << "Add Customer Account" << endl;
			cout << "\nPlease enter the name of the customer: ";
			cin >> name;
			cout << "Please enter the surname of the customer: ";
			cin >> surname;
			cout << "Please enter the contact (Phone Number/E-mail) of the customer: ";
			cin >> contact;
			cout << endl;
			Customer customer(name, surname, contact);
			if (checkDuplicateAccount(&customerList, &customer)) {
				cout << "Customer account already exists!" << endl;
			}
			else {
				customerList.emplace_front(name, surname, contact);
			}
		}
		break;
		case 6:
		{
			cout << "\nAll Accounts:\n" << endl;
			for (Customer& cus : customerList) {
				cout << cus << endl;
			}
		}
		break;
		case 7:
		{
			cout << "EliteDrive system closed.\n";
			break;
		}
		default:
		{
			cout << "Invalid choice. Please try again.\n";
		}
		}

	} while (choice != 7);

	carFile.close();

	return 0;
}