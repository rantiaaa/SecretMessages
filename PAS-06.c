/*	Secret Messages : Encoder and Decoder
	Oleh Kelompok 6: Lauren Christy Tanudjaja - 2106707870
                     Miranti Anggunsari - 2106731472
    Tanggal: 26 April 2022									*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <omp.h>

//Inisialisasi Linked List
struct data {
  char sender[30];
  char receiver[30];
  char letter[30];
  int penerima;
  int urutan;
  struct data * next;
};

//Prototype dari function encoding dan decoding
void encoding(int k,struct data* head);
int decoding();
void mainmenu();
int Node(struct data** head, char penerima[30], char pengirim[30], char isi[30], int m);

int main() {
  system("COLOR F0");
  int select, k, i, m;
  char isi[100], pengirim[100], penerima[100];
  struct data * temp2;
  struct data* head = NULL;
  
  mainmenu();
  while (1) {
	scanf("%d", & select);
    switch (select) {
    case 1:
      system("CLS");
      printf("======================================\n");
      do{
      	printf("Enter Number of Messages: ");
      	scanf("%d", &k);
	  } while(k < 0 || k > 5);
      system("CLS");
      
      for (i = 0; i < k; i++) {//input data
        printf("==========Letter %d===========\n", i+1);
        printf("%d.  From (End with ;): ", i + 1);
        scanf(" %[^\n]s", pengirim);
        printf("    To (End with ;)  : ", i + 1);
        scanf(" %[^\n]s", penerima);
        printf("    Message (End with ===): ", i + 1);
        scanf(" %[^\n]s", isi);

        Node(&head, penerima, pengirim, isi, m); //Fungsi node
        m++; //Input berapa kali
      }
      
      	system("CLS");
      	printf("==============Encyption===============\n");
      	encoding(k, head);
      	system("PAUSE");
      	mainmenu();
      	break;
      	
    //Menu
    case 2:
      	system("CLS");
      	decoding();
      	printf("\n");
      	system("PAUSE");
      	mainmenu();
      	break;

    case 3:
    	printf("\nTerima kasih telah menggunakan program ini!");
      	exit(0);
      	break;
      	
    //Error handling 
    default:
      	printf("Invalid Input!\n");
      	printf("Enter Correct Input: ");
		system("color 4F");
		Sleep(200);
		system("color 0F");
    }
  }
  return 0;
}

//Fungsi enconding
void encoding(int k,struct data* head) {
  struct data * temp3 = head;
  int i, j, key, n1, n2, space, newline, kar;
  char fname[30];

  printf("Enter File pengirim (add .txt) : ");
  scanf(" %[^\n]s", fname);

  FILE * fdata;
  fdata = fopen(fname, "w");

  if (fdata == NULL) {
    perror("Error ");
    system("color 4F");
	Sleep(200);
	system("color 0F");
  }

  //Kunci untuk enkripsi
  printf("Enter Key (Number): ");
  scanf("%d", & key);

  printf("Encryption Result: \n");

  omp_set_num_threads(4); //Set jumlah thread

  #pragma omp parallel 
  {
    int numt, tid, awal, akhir;
    int l;
    numt = omp_get_num_threads();
    tid = omp_get_thread_num();
    awal = (k / numt) * tid;//awal loop
    akhir = (k / numt) * (tid + 1);//akhir loop

    if (tid == numt - 1)
      akhir = k;
    
	#pragma omp barrier //Agar inisialisasi akhir dan awal dapat b berjalan
	
   //Pencegahan race condition
    #pragma omp critical
	  {
      printf("\nThread %d (%d - %d): ", tid, awal, akhir);

      //Loop sampai urutan node sesuai dengan urutan data
      while ((temp3 -> urutan) < awal)
        temp3 = temp3 -> next;
      
      //Print hasil enkripsi ke text file
      for (i = awal; i < akhir; i++) {
        //Memasukkan nama pengirim ke local array
        char sender[30];
        strcpy(sender, temp3 -> sender);

        //Memasukkan nama penerima ke local array
        char receiver[30];
        strcpy(receiver, temp3 -> receiver);

        //Memasukkan isi surat ke local array
        char letter[30];
        strcpy(letter, temp3 -> letter);

        //Memasukkan data pengirim kedalam text file
        for (j = 0; sender[j] != '\0'; j++) {
          kar = sender[j] + key;
          //Menggunakan enkripsi dengan shift ASCII number
          printf("%d ", kar);
          //Print hasil enkripsi
          fprintf(fdata, "%d ", kar);
          //Data disimpan         
        }
	
		//Menggunakan enkripsi dengan shift ASCII number
        for (j = 0; receiver[j] != '\0'; j++) {
          kar = receiver[j] + key;
          //Print hasil enkripsi	
          printf("%d ", kar);
          //Data disimpan
          fprintf(fdata, "%d ", kar);
        }
        
		//Menggunakan enkripsi dengan shift ASCII number
        for (j = 0; letter[j] != '\0'; j++) {
          kar = letter[j] + key;
          //Print hasil enkripsi	
          printf("%d ", kar);
          //Data disimpan
          fprintf(fdata, "%d ", kar);
        }

        //Lanjut ke node berikutnya
        temp3 = temp3 -> next;
        printf("\n");
        fprintf(fdata, "\n");
      }
    }
  }
  fclose(fdata);
  printf("\nFile Has Been Encrypted and Saved as %s\n", fname);
}

//Fungsi untuk decoding
int decoding() {
  char fname1[30], fname2[30];
  char decode[1000] = {0};
  int i = 0, key;
  FILE * fdata1;
  FILE * fdata2;

  //Meminta input kepada user
  printf("==============Decryption==============\n");
  printf("Existing Filename (add .txt): ");
  scanf(" %[^\n]s", fname1);
  printf("Destination Filename (add .txt): ");
  scanf(" %[^\n]s", fname2);

  //Membuka file yang akan di decode
  fdata1 = fopen(fname1, "r");
  if (fdata1 == NULL) {
    perror("Error ");
    return 0;
  }

  //Membuka file untuk menaruh hasil decoding ke dalam file
  fdata2 = fopen(fname2, "w");
  if (fdata2 == NULL) {
    perror("Error ");
    system("color 4F");
	Sleep(200);
	system("color 0F");
    return 0;
  }

  printf("Enter Key: ");
  scanf("%d", & key);

  //Decrypt algorithm
  printf("\nDecrypting...\n\n");
  i = 0;
  #pragma omp parallel
	{
		#pragma omp single
		{
			#pragma omp critical
			{
				  while (fscanf(fdata1, "%d", & decode[i]) != EOF) {
				    decode[i] = decode[i] - key;
				    printf("%c", decode[i]);
				    fprintf(fdata2, "%c", decode[i]);
				    i++;
				  }
			  }
			}
}
  fclose(fdata1);
  fclose(fdata2);
  printf("\nFile Has Been Decrypted and Saved as %s\n", fname2);
}

//Function main menu
void mainmenu() {
  system("CLS");
  printf("======================================\n");
  printf("        Secret Messages Maker\n");
  printf("======================================\n");
  printf("\t    Menu Selection\n");
  printf("\t    1. Data Input\n");
  printf("\t    2. Decryptor\n");
  printf("\t    3. Exit\n");
  printf("======================================\n");
  printf("Selection: ");
}

//Function untuk linked list
int Node(struct data** head, char penerima[30], char pengirim[30], char isi[30], int m) {
  struct data * temp = NULL;
  temp = malloc(sizeof(struct data));
  int i;

  //memasukkan data ke linked list
  temp -> urutan = m;
  strcpy(temp -> letter, isi);
  strcpy(temp -> receiver, penerima);
  strcpy(temp -> sender, pengirim);
  temp -> next = NULL;

  if (m == 1) {
    temp -> next = *head;
    *head = temp;
    return 0;
  }

  struct data * temp1;
  temp1 = *head;

  for (i = 0; i < m - 2; i++)
    temp1 = temp1 -> next;

  temp -> next = temp1 -> next;
  temp1 -> next = temp;

}
