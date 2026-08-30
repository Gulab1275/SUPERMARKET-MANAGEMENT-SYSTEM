#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PRODUCTS 1000
#define MAX_CART_ITEMS 200

struct Product {
    int id;
    char name[50];
    float price;
    int quantity;
};

struct Customer {
    char name[50];
    char contact[20];
};

const char *filename = "products.txt";
const char *adminFile = "admin.txt";
const char *salesFile = "sales.txt";
const char *cartFile = "cart.txt";

int isValidContact(const char *c);
void formatName(char *name);
void initializeProducts();
void initializeAdmin();
int checkAdminPassword();
void adminMenu();
void userMenu();
void addProduct();
void viewProducts();
void updateProduct();
void deleteProduct();
void searchProduct();
void viewSalesReport();
void buyProduct();
void generateBill(struct Customer c);
void trim(char *s);
int loadProducts(struct Product products[], int max);
int saveProducts(struct Product products[], int count);

int main() {
    initializeProducts();
    initializeAdmin();
    int choice;
    while (1) {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        printf("\n===== SUPERMARKET MANAGEMENT SYSTEM =====\n");
        printf("1. Admin Login\n");
        printf("2. User Section\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("Invalid input!\n");
            continue;
        }
        switch (choice) {
        case 1:
            if (checkAdminPassword())
                adminMenu();
            else {
                printf("Incorrect password!\n");
#ifdef _WIN32
                system("pause");
#endif
            }
            break;
        case 2:
            userMenu();
            break;
        case 3:
            printf("Thank you for using the system!\n");
            exit(0);
        default:
            printf("Invalid choice! Try again.\n");
        }
    }
    return 0;
}

void trim(char *s) {
    if (!s) return;
    size_t len = strlen(s);
    while (len > 0 && (s[len-1] == '\n' || s[len-1] == '\r' || isspace((unsigned char)s[len-1]))) {
        s[len-1] = '\0';
        len--;
    }
    size_t start = 0;
    while (s[start] && isspace((unsigned char)s[start])) start++;
    if (start > 0) memmove(s, s + start, strlen(s + start) + 1);
}

int loadProducts(struct Product products[], int max) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return 0;
    char line[256];
    int count = 0;
    while (fgets(line, sizeof(line), fp) && count < max) {
        struct Product p;
        if (sscanf(line, "%d|%49[^|]|%f|%d", &p.id, p.name, &p.price, &p.quantity) == 4) {
            trim(p.name);
            products[count++] = p;
        }
    }
    fclose(fp);
    return count;
}

int saveProducts(struct Product products[], int count) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return 0;
    for (int i = 0; i < count; ++i) {
        fprintf(fp, "%d|%s|%.2f|%d\n",
                products[i].id,
                products[i].name,
                products[i].price,
                products[i].quantity);
    }
    fclose(fp);
    return 1;
}

void initializeProducts() {
    FILE *fp = fopen(filename, "r");
    if (fp) { fclose(fp); return; }
    fp = fopen(filename, "w");
    struct Product p[5] = {
        {1, "Milk", 120, 50},
        {2, "Bread", 50, 40},
        {3, "Rice", 80, 100},
        {4, "Sugar", 60, 80},
        {5, "Oil", 150, 50}
    };
    for (int i = 0; i < 5; i++)
        fprintf(fp, "%d|%s|%.2f|%d\n", p[i].id, p[i].name, p[i].price, p[i].quantity);
    fclose(fp);
}

void initializeAdmin() {
    FILE *fp = fopen(adminFile, "r");
    if (fp) { fclose(fp); return; }
    fp = fopen(adminFile, "w");
    fprintf(fp, "admin admin123\n");
    fclose(fp);
}

int checkAdminPassword() {
    char password[50] = "admin123";
    FILE *fp = fopen(adminFile, "r");
    if (fp) {
        fscanf(fp, "%*s %49s", password);
        fclose(fp);
    }
    char entered[50];
    printf("\n===== ADMIN LOGIN =====\n");
    printf("Enter Password: ");
    scanf("%49s", entered);
    return strcmp(entered, password) == 0;
}

void adminMenu() {
    int choice;
    while (1) {
        printf("\n===== ADMIN MENU =====\n");
        printf("1. Add Product (enter ID manually)\n");
        printf("2. View All Products\n");
        printf("3. Update Product\n");
        printf("4. Delete Product\n");
        printf("5. Search Product\n");
        printf("6. View Sales Report\n");
        printf("7. Back to Main Menu\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("Invalid input!\n");
            continue;
        }
        switch (choice) {
        case 1: addProduct(); break;
        case 2: viewProducts(); break;
        case 3: updateProduct(); break;
        case 4: deleteProduct(); break;
        case 5: searchProduct(); break;
        case 6: viewSalesReport(); break;
        case 7: return;
        default: printf("Invalid choice!\n");
        }
    }
}

void viewSalesReport() {
    FILE *fp = fopen(salesFile, "r");
    if (!fp) {
        printf("\nNo sales records found.\n");
        return;
    }
    printf("\n========== SALES REPORT ==========\n");
    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
    }
    printf("==================================\n");
    fclose(fp);
}

void addProduct() {
    struct Product products[MAX_PRODUCTS];
    int count = loadProducts(products, MAX_PRODUCTS);
    int id;
    printf("\nEnter Product ID (integer): ");
    if (scanf("%d", &id) != 1) {
        while (getchar() != '\n');
        printf("Invalid ID input.\n");
        return;
    }
    while (getchar() != '\n' && !feof(stdin));
    int index = -1;
    for (int i = 0; i < count; ++i) {
        if (products[i].id == id) {
            index = i;
            break;
        }
    }
    if (index >= 0) {
        printf("Product with ID %d already exists:\n", id);
        printf("ID: %d | Name: %s | Price: %.2f | Qty: %d\n",
               products[index].id, products[index].name,
               products[index].price, products[index].quantity);
        printf("\nChoose action:\n");
        printf("1. Overwrite (replace name, price, quantity)\n");
        printf("2. Increase quantity (add to existing quantity)\n");
        printf("3. Cancel\n");
        printf("Enter choice: ");
        int achoice;
        if (scanf("%d", &achoice) != 1) {
            while (getchar() != '\n');
            printf("Invalid input.\n");
            return;
        }
        while (getchar() != '\n' && !feof(stdin));
        if (achoice == 3) {
            printf("Operation cancelled.\n");
            return;
        }
        char name[50];
        float price;
        int qty;
        printf("Enter Product Name: ");
        if (!fgets(name, sizeof(name), stdin)) { printf("Input error.\n"); return; }
        trim(name);
        printf("Enter Product Price: ");
        if (scanf("%f", &price) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; }
        printf("Enter Quantity: ");
        if (scanf("%d", &qty) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; }
        if (achoice == 1) {
            strncpy(products[index].name, name, sizeof(products[index].name));
            products[index].price = price;
            products[index].quantity = qty;
            printf("? Product overwritten.\n");
        } else if (achoice == 2) {
            products[index].quantity += qty;
            strncpy(products[index].name, name, sizeof(products[index].name));
            products[index].price = price;
            printf("? Quantity increased. New qty = %d\n", products[index].quantity);
        }
        if (!saveProducts(products, count)) {
            printf("Error saving products.\n");
        }
        return;
    }
    struct Product np;
    np.id = id;
    printf("Enter Product Name: ");
    if (!fgets(np.name, sizeof(np.name), stdin)) { printf("Input error.\n"); return; }
    trim(np.name);
    if (strlen(np.name) == 0) {
        printf("Product name cannot be empty.\n");
        return;
    }
    printf("Enter Product Price: ");
    if (scanf("%f", &np.price) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; }
    printf("Enter Quantity: ");
    if (scanf("%d", &np.quantity) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; }
    if (count < MAX_PRODUCTS) {
        products[count++] = np;
        if (saveProducts(products, count)) {
            printf("? Product added with ID %d.\n", np.id);
        } else {
            printf("Error saving product file.\n");
        }
    } else {
        printf("Product storage is full.\n");
    }
}

void viewProducts() {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("No products available.\n");
        return;
    }
    struct Product p;
    char line[200];
    printf("\n%-10s %-25s %-10s %-10s\n", "ID", "Name", "Price", "Qty");
    printf("-------------------------------------------------------------\n");
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%d|%49[^|]|%f|%d", &p.id, p.name, &p.price, &p.quantity) == 4) {
            trim(p.name);
            printf("%-10d %-25s %-10.2f %-10d", p.id, p.name, p.price, p.quantity);
            if (p.quantity < 10) printf(" <-- LOW STOCK!");
            printf("\n");
        }
    }
    fclose(fp);
}

void updateProduct() {
    struct Product products[MAX_PRODUCTS];
    int count = loadProducts(products, MAX_PRODUCTS);
    if (count == 0) { printf("No products to update.\n"); return; }
    int id;
    printf("Enter Product ID to update: ");
    if (scanf("%d", &id) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; }
    int idx = -1;
    for (int i = 0; i < count; ++i) if (products[i].id == id) { idx = i; break; }
    if (idx < 0) { printf("Product not found.\n"); return; }
    while (getchar() != '\n' && !feof(stdin));
    printf("Enter new name: ");
    if (!fgets(products[idx].name, sizeof(products[idx].name), stdin)) { printf("Input error.\n"); return; }
    trim(products[idx].name);
    printf("Enter new price: ");
    if (scanf("%f", &products[idx].price) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; }
    printf("Enter new quantity: ");
    if (scanf("%d", &products[idx].quantity) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; }
    if (saveProducts(products, count)) printf("? Product updated successfully!\n");
    else printf("Error saving products.\n");
}

void deleteProduct() {
    struct Product products[MAX_PRODUCTS];
    int count = loadProducts(products, MAX_PRODUCTS);
    if (count == 0) { printf("No products to delete.\n"); return; }
    int id;
    printf("Enter Product ID to delete: ");
    if (scanf("%d", &id) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; }
    int newCount = 0;
    int found = 0;
    for (int i = 0; i < count; ++i) {
        if (products[i].id == id) { found = 1; continue; }
        products[newCount++] = products[i];
    }
    if (!found) { printf("Product not found.\n"); return; }
    if (saveProducts(products, newCount)) printf("? Product deleted successfully!\n");
    else printf("Error saving products.\n");
}

void searchProduct() {
    struct Product p;
    FILE *fp = fopen(filename, "r");
    if (!fp) { printf("No products.\n"); return; }
    int id;
    printf("Enter Product ID to search: ");
    if (scanf("%d", &id) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); fclose(fp); return; }
    int found = 0;
    char line[200];
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%d|%49[^|]|%f|%d", &p.id, p.name, &p.price, &p.quantity) == 4) {
            if (p.id == id) {
                trim(p.name);
                printf("\n%-10s %-25s %-10s %-10s\n", "ID", "Product Name", "Quantity", "Price");
                printf("--------------------------------------------------------------\n");
                printf("%-10d %-25s %-10d %-10.2f\n", p.id, p.name, p.quantity, p.price);
                if (p.quantity < 10) printf(" <-- LOW STOCK!\n");
                found = 1;
                break;
            }
        }
    }
    if (!found) printf("? Product not found!\n");
    fclose(fp);
}

void userMenu() {
    int choice;
    struct Customer c;
    printf("\n===== CUSTOMER DETAILS =====\n");
    while (getchar() != '\n' && !feof(stdin));
    printf("Enter Customer Name: ");
    if (!fgets(c.name, sizeof(c.name), stdin)) { printf("Input error.\n"); return; }
    formatName(c.name);
    while (1) {
        printf("Enter Contact Number (11 digits): ");
        if (scanf("%19s", c.contact) != 1) {
            while (getchar() != '\n');
            printf("Invalid input!\n");
            continue;
        }
        if (!isValidContact(c.contact)) {
            printf("Invalid! Contact number must be exactly 11 digits and numeric.\n");
            continue;
        }
        break;
    }
    FILE *cart = fopen(cartFile, "w");
    if (cart) fclose(cart);
    while (1) {
        printf("\n===== USER MENU =====\n");
        printf("1. View Products\n");
        printf("2. Buy Products (Add to Cart)\n");
        printf("3. Generate Bill\n");
        printf("4. Back to Main Menu\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("Invalid input!\n");
            continue;
        }
        switch (choice) {
        case 1: viewProducts(); break;
        case 2: buyProduct(); break;
        case 3: generateBill(c); break;
        case 4: return;
        default: printf("Invalid choice!\n");
        }
    }
}

void buyProduct() {
    struct Product p;
    char line[256];
    int id, qty;
    int addMore = 0;
    do {
        FILE *fp = fopen(filename, "r");
        if (!fp) {
            printf("Error opening product file!\n");
            return;
        }
        printf("\nEnter Product ID to add to cart: ");
        if (scanf("%d", &id) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); fclose(fp); return; }
        printf("Enter Quantity: ");
        if (scanf("%d", &qty) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); fclose(fp); return; }
        int found = 0;
        rewind(fp);
        while (fgets(line, sizeof(line), fp)) {
            if (sscanf(line, "%d|%49[^|]|%f|%d", &p.id, p.name, &p.price, &p.quantity) == 4) {
                if (p.id == id) {
                    found = 1;
                    break;
                }
            }
        }
        fclose(fp);
        if (!found) {
            printf("? Product not found!\n");
            printf("\nAdd another item? (1 = Yes / 0 = No): ");
            if (scanf("%d", &addMore) != 1) { addMore = 0; while (getchar() != '\n'); }
            continue;
        }
        if (qty > p.quantity) {
            printf("? Not enough stock! Only %d left.\n", p.quantity);
            printf("\nAdd another item? (1 = Yes / 0 = No): ");
            if (scanf("%d", &addMore) != 1) { addMore = 0; while (getchar() != '\n'); }
            continue;
        }
        FILE *cart = fopen(cartFile, "r");
        FILE *temp = fopen("temp_cart.txt", "w");
        int existed = 0;
        if (!temp) {
            if (cart) fclose(cart);
            printf("Error opening temp cart file.\n");
            return;
        }
        char c_line[256];
        if (cart) {
            while (fgets(c_line, sizeof(c_line), cart)) {
                int c_id, c_qty;
                float c_price, c_total;
                char c_name[50];
                if (sscanf(c_line, "%d|%49[^|]|%f|%d|%f", &c_id, c_name, &c_price, &c_qty, &c_total) == 5) {
                    if (c_id == id) {
                        c_qty += qty;
                        c_total = c_qty * c_price;
                        existed = 1;
                    }
                    fprintf(temp, "%d|%s|%.2f|%d|%.2f\n", c_id, c_name, c_price, c_qty, c_total);
                }
            }
            fclose(cart);
        }
        if (!existed) {
            fprintf(temp, "%d|%s|%.2f|%d|%.2f\n", p.id, p.name, p.price, qty, p.price * qty);
        }
        fclose(temp);
        remove(cartFile);
        rename("temp_cart.txt", cartFile);
        printf("? Product added to cart successfully!\n");
        printf("\nAdd another item? (1 = Yes / 0 = No): ");
        if (scanf("%d", &addMore) != 1) { addMore = 0; while (getchar() != '\n'); }
    } while (addMore == 1);
}

void generateBill(struct Customer c) {
    FILE *cart = fopen(cartFile, "r");
    if (!cart) {
        printf("Error opening cart file.\n");
        return;
    }
    fseek(cart, 0, SEEK_END);
    long size = ftell(cart);
    if (size == 0) {
        fclose(cart);
        printf("Your cart is empty.\n");
        return;
    }
    rewind(cart);
    struct {
        int id;
        char name[50];
        float price;
        int qty;
        float total;
    } items[MAX_CART_ITEMS];
    int count = 0;
    char line[256];
    while (fgets(line, sizeof(line), cart) && count < MAX_CART_ITEMS) {
        if (sscanf(line, "%d|%49[^|]|%f|%d|%f",
                   &items[count].id,
                   items[count].name,
                   &items[count].price,
                   &items[count].qty,
                   &items[count].total) == 5) {
            trim(items[count].name);
            count++;
        }
    }
    fclose(cart);
    float grandTotal = 0.0f;
    printf("\n=========== BILL ===========\n");
    printf("Customer: %s\nContact: %s\n", c.name, c.contact);
    printf("-----------------------------\n");
    printf("%-5s %-20s %-8s %-6s %-8s\n", "ID", "Product", "Price", "Qty", "Total");
    printf("-----------------------------\n");
    for (int i = 0; i < count; ++i) {
        printf("%-5d %-20s %-8.2f %-6d %-8.2f\n",
               items[i].id, items[i].name, items[i].price, items[i].qty, items[i].total);
        grandTotal += items[i].total;
    }
    printf("-----------------------------\n");
    printf("Subtotal: Rs %.2f\n", grandTotal);
    float discount = 0;
    if (grandTotal > 5000) {
        discount = grandTotal * 0.05f;
        grandTotal -= discount;
        printf("Discount (5%%): -Rs %.2f\n", discount);
    }
    printf("Final Total: Rs %.2f\n", grandTotal);
    printf("=============================\n");
    FILE *sf = fopen(salesFile, "a");
    if (sf) {
        fprintf(sf, "\nCustomer: %s\nContact: %s\n", c.name, c.contact);
        for (int i = 0; i < count; ++i) {
            fprintf(sf, "%d|%s|%.2f|%d|%.2f\n",
                    items[i].id, items[i].name, items[i].price, items[i].qty, items[i].total);
        }
        fprintf(sf, "Subtotal: %.2f | Discount: %.2f | Final: %.2f\n",
                grandTotal + discount, discount, grandTotal);
        fprintf(sf, "-----------------------------\n");
        fclose(sf);
    }
    struct Product products[MAX_PRODUCTS];
    int pcount = loadProducts(products, MAX_PRODUCTS);
    for (int i = 0; i < pcount; ++i) {
        for (int j = 0; j < count; ++j) {
            if (products[i].id == items[j].id) {
                products[i].quantity -= items[j].qty;
                if (products[i].quantity < 0) products[i].quantity = 0;
            }
        }
    }
    if (!saveProducts(products, pcount)) {
        printf("Error updating product file.\n");
    }
    remove(cartFile);
    printf("? Bill generated, stock updated, and cart cleared.\n");
}

int isValidContact(const char *c) {
    if (!c) return 0;
    int len = strlen(c);
    if (len != 11) return 0;
    for (int i = 0; i < 11; i++) {
        if (!isdigit((unsigned char)c[i])) return 0;
    }
    return 1;
}

void formatName(char *name) {
    trim(name);
    if (strlen(name) == 0) return;
    name[0] = toupper((unsigned char)name[0]);
    for (int i = 1; name[i]; i++) {
        name[i] = tolower((unsigned char)name[i]);
    }
}
