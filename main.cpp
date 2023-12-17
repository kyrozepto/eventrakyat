#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <ctime>

using namespace std;

struct Event
{
    string name;
    string date;
    string time;
    string location;
    int capacity;
    string description;
    string category;
};

vector<Event> events; // Menyimpan semua acara

// Fungsi untuk menentukan apakah suatu tanggal adalah hari ini
bool isToday(const string &date)
{
    time_t now = time(0);
    tm *currentDate = localtime(&now);

    // Format tanggal di file events.txt: dd-mm-yyyy
    int day, month, year;
    sscanf(date.c_str(), "%d-%d-%d", &day, &month, &year);

    return currentDate->tm_mday == day && (currentDate->tm_mon + 1) == month && (currentDate->tm_year + 1900) == year;
}

void saveEventsToFile()
{
    ofstream file("events.txt");

    if (file.is_open())
    {
        for (const Event &e : events)
        {
            file << e.name << "\n";
            file << e.date << "\n";
            file << e.time << "\n";
            file << e.location << "\n";
            file << e.capacity << "\n";
            file << e.description << "\n";
            file << e.category << "\n";
        }

        file.close();
        cout << "Acara disimpan.\n";
    }
    else
    {
        cout << "Gagal membuka file untuk penyimpanan.\n";
    }
}

void loadEventsFromFile()
{
    ifstream file("events.txt");

    if (file.is_open())
    {
        events.clear(); // Menghapus acara yang ada sebelumnya

        while (!file.eof())
        {
            Event e;
            getline(file, e.name);
            getline(file, e.date);
            getline(file, e.time);
            getline(file, e.location);
            file >> e.capacity;
            file.ignore(); // Membersihkan newline dari buffer
            getline(file, e.description);
            getline(file, e.category);

            if (!e.name.empty())
            {
                events.push_back(e);
            }
        }

        file.close();
    }
    else
    {
        cout << "Gagal membuka file untuk pengambilan data acara.\n";
    }
}

void viewEventDetails(int index)
{
    const Event &selectedEvent = events[index];

    cout << "\nDetail Acara:\n";
    cout << "Nama Acara: " << selectedEvent.name << "\n";
    cout << "Tanggal: " << selectedEvent.date << "\n";
    cout << "Waktu: " << selectedEvent.time << "\n";
    cout << "Tempat: " << selectedEvent.location << "\n";
    cout << "Kapasitas: " << selectedEvent.capacity << "\n";
    cout << "Deskripsi: " << selectedEvent.description << "\n";
    cout << "Kategori: " << selectedEvent.category << "\n";

    cout << "\n0. Keluar\n";
    cout << "Pilih opsi (0): ";
    int choice;
    cin >> choice;
    cin.ignore(); // Membersihkan newline dari buffer
}

void searchEvent()
{
    loadEventsFromFile();

    cout << "Nama Acara: ";
    string eventName;
    getline(cin, eventName);

    cout << "Kota/Lokasi: ";
    string eventLocation;
    getline(cin, eventLocation);

    cout << "Jadwal Acara (0. Semua, 1. Hari ini, 2. Minggu ini): ";
    string eventScheduleStr;
    getline(cin, eventScheduleStr);

    // Convert the schedule input to an integer, or default to 0 if blank
    int eventSchedule = eventScheduleStr.empty() ? 0 : stoi(eventScheduleStr);

    // Mengubah nama acara, lokasi, dan jadwal menjadi huruf kecil
    transform(eventName.begin(), eventName.end(), eventName.begin(), ::tolower);
    transform(eventLocation.begin(), eventLocation.end(), eventLocation.begin(), ::tolower);

    vector<int> matchingEvents; // Menyimpan indeks acara yang sesuai

    for (size_t i = 0; i < events.size(); ++i)
    {
        // Mengubah informasi acara menjadi huruf kecil
        string currentEventNameLower = events[i].name;
        string currentEventLocationLower = events[i].location;
        string currentEventDate = events[i].date;

        transform(currentEventNameLower.begin(), currentEventNameLower.end(), currentEventNameLower.begin(), ::tolower);
        transform(currentEventLocationLower.begin(), currentEventLocationLower.end(), currentEventLocationLower.begin(), ::tolower);

        // Mencocokkan nama acara
        bool matchName = eventName.empty() || currentEventNameLower.find(eventName) != string::npos;
        // Mencocokkan lokasi acara
        bool matchLocation = eventLocation.empty() || currentEventLocationLower.find(eventLocation) != string::npos;
        // Mencocokkan jadwal acara
        bool matchSchedule = (eventSchedule == 0) || (eventSchedule == 1 && isToday(currentEventDate)) ||
                             (eventSchedule == 2 && isToday(currentEventDate)); // Ubah menjadi kondisi yang sesuai

        // Jika semua kriteria terpenuhi, tambahkan ke hasil pencarian
        if (matchName && matchLocation && matchSchedule)
        {
            matchingEvents.push_back(i);
        }
    }

    if (matchingEvents.empty())
    {
        cout << "Tidak ada acara yang sesuai dengan kriteria pencarian.\n";
    }
    else
    {
        cout << "Hasil Pencarian:\n";
        for (size_t i = 0; i < matchingEvents.size(); ++i)
        {
            const Event &e = events[matchingEvents[i]];
            cout << i + 1 << ". " << e.name << "\n";
            cout << "   Kategori: " << e.category << "\n";
            cout << "   Tanggal: " << e.date << " " << e.time << " WIB - " << e.location << "\n";
        }

        cout << "Masukkan nomor acara untuk melihat detail (0 untuk keluar): ";
        int eventIndex;
        cin >> eventIndex;
        cin.ignore(); // Membersihkan newline dari buffer

        if (eventIndex > 0 && static_cast<size_t>(eventIndex) <= matchingEvents.size())
        {
            viewEventDetails(matchingEvents[eventIndex - 1]);
        }
        else if (eventIndex != 0)
        {
            cout << "Nomor acara tidak valid.\n";
        }
    }
}

// Fungsi untuk menampilkan daftar semua event
void listEvents()
{
    loadEventsFromFile();

    cout << "Daftar Acara:\n";
    for (size_t i = 0; i < events.size(); ++i)
    {
        const Event &e = events[i];
        cout << i + 1 << ". " << e.name << "\n";
        cout << "   Kategori: " << e.category << "\n";
        cout << "   Tanggal: " << e.date << " - " << e.time << " WIB - " << e.location << "\n";
    }
}

// Fungsi untuk membuat event baru
void createEvent()
{
    // Membuat acara baru
    Event newEvent;

    cout << "Nama acara: ";
    getline(cin, newEvent.name);

    cout << "Tanggal: ";
    getline(cin, newEvent.date);

    cout << "Waktu: ";
    getline(cin, newEvent.time);

    cout << "Tempat: ";
    getline(cin, newEvent.location);

    cout << "Kapasitas: ";
    cin >> newEvent.capacity;
    cin.ignore(); // Membersihkan newline dari buffer

    // Menambahkan baris kosong tambahan untuk memastikan newline sepenuhnya diabaikan
    cout << endl;

    cout << "Deskripsi: ";
    getline(cin, newEvent.description);

    cout << "Kategori: ";
    getline(cin, newEvent.category);

    events.push_back(newEvent);

    saveEventsToFile();
}

// Fungsi untuk mengedit event
void editEvent(int index)
{
    Event &editedEvent = events[index];

    cout << "Edit Acara:\n";
    cout << "1. Nama acara: " << editedEvent.name << "\n";
    cout << "2. Tanggal: " << editedEvent.date << "\n";
    cout << "3. Waktu: " << editedEvent.time << "\n";
    cout << "4. Tempat: " << editedEvent.location << "\n";
    cout << "5. Kapasitas: " << editedEvent.capacity << "\n";
    cout << "6. Deskripsi: " << editedEvent.description << "\n";
    cout << "7. Kategori: " << editedEvent.category << "\n";
    cout << "0. Selesai\n";
    cout << "Pilih opsi (0-7): ";

    int editChoice;
    cin >> editChoice;
    cin.ignore(); // Membersihkan newline dari buffer

    switch (editChoice)
    {
    case 1:
        cout << "Nama acara: ";
        getline(cin, editedEvent.name);
        break;
    case 2:
        cout << "Tanggal: ";
        getline(cin, editedEvent.date);
        break;
    case 3:
        cout << "Waktu: ";
        getline(cin, editedEvent.time);
        break;
    case 4:
        cout << "Tempat: ";
        getline(cin, editedEvent.location);
        break;
    case 5:
        cout << "Kapasitas: ";
        cin >> editedEvent.capacity;
        cin.ignore(); // Membersihkan newline dari buffer
        break;
    case 6:
        cout << "Deskripsi: ";
        getline(cin, editedEvent.description);
        break;
    case 7:
        cout << "Kategori: ";
        getline(cin, editedEvent.category);
        break;
    case 0:
        cout << "Perubahan disimpan.\n";
        break;
    default:
        cout << "Opsi tidak valid. Silakan coba lagi.\n";
        break;
    }
}

// Fungsi untuk mengelola acara
void manageEvent()
{
    loadEventsFromFile();

    cout << "Pengelolaan Acara:\n";
    cout << "1. Lihat Daftar Acara\n";
    cout << "2. Edit Acara\n";
    cout << "0. Kembali\n";
    cout << "Pilih opsi (0-2): ";

    int manageChoice;
    cin >> manageChoice;
    cin.ignore(); // Membersihkan newline dari buffer

    switch (manageChoice)
    {
    case 1:
        listEvents();
        break;
    case 2:
        // Meminta pengguna memilih acara yang akan diedit
        cout << "Masukkan nomor acara yang ingin diedit: ";
        int editIndex;
        cin >> editIndex;
        cin.ignore(); // Membersihkan newline dari buffer

        // Memastikan nomor acara valid
        if (editIndex > 0 && static_cast<size_t>(editIndex) <= events.size())
        {
            // Mengedit acara
            editEvent(editIndex - 1);
            // Menyimpan perubahan ke dalam file
            saveEventsToFile();
        }
        else
        {
            cout << "Nomor acara tidak valid.\n";
        }
        break;
    case 0:
        cout << "Kembali ke menu utama.\n";
        break;
    default:
        cout << "Opsi tidak valid. Silakan coba lagi.\n";
        break;
    }
}

// Fungsi untuk menu login
int login()
{
    cout << "Login sebagai:\n";
    cout << "1. User\n";
    cout << "2. Admin\n";
    cout << "0. Keluar\n";
    cout << "Pilih opsi (0-2): ";

    int loginChoice;
    cin >> loginChoice;
    cin.ignore(); // Membersihkan newline dari buffer

    return loginChoice;
}

// Fungsi utama
int main()
{
    int loginChoice;
    int userChoice;

    do
    {
        loginChoice = login();

        switch (loginChoice)
        {
        case 1:
            // User menu
            cout << "\nMenu User:\n";
            cout << "1. Cari Acara\n";
            cout << "2. Daftar Acara\n";
            cout << "3. Keluar\n";
            cout << "Pilih opsi (1-3): ";
            cin >> userChoice;
            cin.ignore(); // Membersihkan newline dari buffer

            switch (userChoice)
            {
            case 1:
                searchEvent();
                break;
            case 2:
                listEvents();
                break;
            case 3:
                cout << "Keluar dari menu user.\n";
                break;
            default:
                cout << "Opsi tidak valid. Silakan coba lagi.\n";
                break;
            }
            break;
        case 2:
            // Admin menu
            cout << "\nMenu Admin:\n";
            cout << "1. Buat Acara\n";
            cout << "2. Pengelolaan Acara\n";
            cout << "3. Keluar\n";
            cout << "Pilih opsi (1-3): ";
            cin >> userChoice;
            cin.ignore(); // Membersihkan newline dari buffer

            switch (userChoice)
            {
            case 1:
                createEvent();
                break;
            case 2:
                manageEvent();
                break;
            case 3:
                cout << "Keluar dari menu admin.\n";
                break;
            default:
                cout << "Opsi tidak valid. Silakan coba lagi.\n";
                break;
            }
            break;
        case 0:
            cout << "Keluar dari program.\n";
            break;
        default:
            cout << "Opsi tidak valid. Silakan coba lagi.\n";
            break;
        }

    } while (loginChoice != 0);

    return 0;
}
