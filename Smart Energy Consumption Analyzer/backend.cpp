#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <iomanip>
#include <algorithm>
#include <limits>
using namespace std;

template<typename T>T monthlyEnergy(T w,T h){return w*h*30/static_cast<T>(1000);}

struct Device{string name,room;double watt,hours;double monthlyKwh()const{return monthlyEnergy(watt,hours);}double monthlyCost(double t)const{return monthlyKwh()*t;}};

class SmartEnergyAnalyzer {
private:
    vector<Device*> devices;
    double solarKW;
    double batteryPercent;
    double tariffINR;      
    string location;

public:
    SmartEnergyAnalyzer(double s,double b,double t=8.0,const string& l="India")
        :solarKW(s),batteryPercent(b),tariffINR(t),location(l){}
    ~SmartEnergyAnalyzer() { for (auto d : devices) delete d; }

    void addDevice(const string& name,const string& room,double watt,double hours){if(watt<=0||hours<=0||hours>24)throw invalid_argument("Invalid: "+name);devices.push_back(new Device{name,room,watt,hours});}

    double totalConsumption() const {
        if (devices.empty()) throw runtime_error("No devices added.");
        double total = 0;
        for (const auto d : devices) total += d->monthlyKwh();
        return total;
    }

    double solarGeneration()const{return solarKW*5*30;}

    double gridUsage() const {
        return max(0.0, totalConsumption() - solarGeneration());
    }

    double monthlyBillINR()const{return gridUsage()*tariffINR;}

    double monthlySavingsINR() const {
        return min(totalConsumption(), solarGeneration()) * tariffINR;
    }

    double solarCoverage() const {
        double total = totalConsumption();
        return (total == 0) ? 0 : min(100.0, (solarGeneration() / total) * 100.0);
    }

    string topDevice() const {
        if (devices.empty()) return "None";
        auto top = *max_element(devices.begin(), devices.end(),
            [](Device* a, Device* b){return a->monthlyKwh() < b->monthlyKwh();});
        return top->name;
    }

    double batteryHoursRemaining()const{return batteryPercent*0.18;}

    void printSummary() const {
        cout << "\n=== SMART ENERGY ANALYZER REPORT ===\n" << fixed << setprecision(2);
        cout << "Location: " << location << " | Solar: " << solarKW << "kW | Battery: " << batteryPercent << "%\n";
        cout << "Total: " << totalConsumption() << "kWh | Solar Gen: " << solarGeneration() << "kWh | Bill: ₹" << monthlyBillINR() << " | Savings: ₹" << monthlySavingsINR() << "\nTop: " << topDevice() << "\n\n";
        vector<Device*> s(devices);
        sort(s.begin(), s.end(), [](Device* a, Device* b){return a->monthlyKwh() > b->monthlyKwh();});
        for (auto d : s) cout << d->name << ": " << d->monthlyKwh() << "kWh (₹" << d->monthlyCost(tariffINR) << ")\n";
    }

    void generateOutput() const {
        ofstream out("output.json");
        if (!out.is_open()) throw runtime_error("Cannot open output.json.");

        out << fixed << setprecision(2) << "{\n" << "  \"location\": \"" << location << "\",\n"
            << "  \"currency\": \"INR\",\n" << "  \"tariff_inr_per_kwh\": " << tariffINR << ",\n"
            << "  \"solar_capacity_kw\": " << solarKW << ",\n" << "  \"battery_percent\": " << batteryPercent << ",\n"
            << "  \"battery_hours_remaining\": " << batteryHoursRemaining() << ",\n"
            << "  \"total_consumption_kwh\": " << totalConsumption() << ",\n"
            << "  \"solar_generation_kwh\": " << solarGeneration() << ",\n"
            << "  \"grid_usage_kwh\": " << gridUsage() << ",\n"
            << "  \"solar_coverage_pct\": " << solarCoverage() << ",\n"
            << "  \"monthly_bill_inr\": " << monthlyBillINR() << ",\n"
            << "  \"monthly_savings_inr\": " << monthlySavingsINR() << ",\n"
            << "  \"top_device\": \"" << topDevice() << "\",\n" << "  \"devices\": [\n";
        for (size_t i = 0; i < devices.size(); ++i) {
            const Device* d = devices[i];
            out << "    {\n" << "      \"name\": \"" << d->name << "\",\n"
                << "      \"room\": \"" << d->room << "\",\n" << "      \"watt\": " << d->watt << ",\n"
                << "      \"hours_per_day\": " << d->hours << ",\n" << "      \"monthly_kwh\": " << d->monthlyKwh() << ",\n"
                << "      \"monthly_cost_inr\": " << d->monthlyCost(tariffINR) << "\n" << "    }" << (i + 1 < devices.size() ? "," : "") << "\n";
        }
        out << "  ]\n}\n";
        out.close();
    }
};
double safeDouble(const string& p, double m = 0) {
    double v;
    while (true) {
        cout << p;
        if (cin >> v && v >= m) break;
        cout << "Invalid input\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return v;
}

string safeLine(const string& p) {
    string v;
    cout << p;
    getline(cin, v);
    return v;
}

int main() {
    try {
        cout << "\n=== SMART ENERGY ANALYZER SETUP ===\n";
        double solar = safeDouble("Solar(kW): ", 0.1);
        double battery = safeDouble("Battery(%): ", 0.0);
        double tariff = safeDouble("Tariff(₹/kWh): ", 0.1);
        string location = safeLine("Location: ");
        if (location.empty()) location = "India";
        SmartEnergyAnalyzer system(solar, battery, tariff, location);
        int n = (int)safeDouble("Devices: ", 1.0);
        for (int i = 0; i < n; ++i) {
            cout << "\nDevice " << (i + 1) << ":\n";
            string name = safeLine("Name: ");
            string room = safeLine("Room: ");
            double watt = safeDouble("Watts: ", 1.0);
            double hours = safeDouble("Hours/day: ", 0.1);
            try {
                system.addDevice(name, room, watt, hours);
                cout << "+ Added\n";
            } catch (const invalid_argument& e) {
                cout << "- " << e.what() << "\n";
            }
        }
        system.printSummary();
        system.generateOutput();
        cout << "Saved\n\n";
    } catch (const runtime_error& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    } catch (...) {
        cerr << "Error\n";
        return 1;
    }
    return 0;
}