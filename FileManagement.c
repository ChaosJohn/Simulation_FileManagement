#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 

#define NAMESIZE 256 
#define CMDSIZE 64 

typedef struct _File {
  int id; 
  char fileName[NAMESIZE]; 
} FileNode, *FilePointer; 

typedef struct _ufd {
  char fileName[NAMESIZE]; 
  int canRead; 
  int canWrite; 
  int canExec; 
  int length; 
  struct _ufd *next; 
} UFDNode, *UFDPointer, *UFDList; 

typedef struct _mdf {
  char userName[NAMESIZE]; 
  UFDList directoryPointer; 
  struct _mdf *next; 
} MDFNode, *MDFPointer, *MDFList; 

typedef struct _afd {
  char userName[NAMESIZE]; 
  char fileName[NAMESIZE]; 
  UFDPointer ufdPointer; 
  int filePos; 
  struct _afd *next; 
} AFDNode, *AFDPointer, *AFDList; 

MDFList mdfList; 
MDFPointer currentMDF; 
AFDList afdList; 
UFDPointer directoryPointer; 


void 
printCmds() {
  printf("\n"); 
  printf("$ help\t\t帮助\n"); 
  printf("$ new \t\t新建用户\n"); 
  printf("$ use \t\t用户登入\n"); 
  printf("$ lsuser\t用户列表\n"); 
  printf("$ create\t创建文件\n"); 
  printf("$ delete\t删除文件\n"); 
  printf("$ open\t\t打开文件\n"); 
  printf("$ close\t\t关闭文件\n"); 
  printf("$ read\t\t读文件\n"); 
  printf("$ write\t\t写文件\n"); 
  printf("$ lsfile\t显示文件列表\n"); 
  printf("$ lsopened\t显示已经打开的文件列表\n"); 
  printf("$ exit\t\t退出到上一级或者退出程序\n"); 
  printf("\n"); 
}


void 
init() {
  mdfList = (MDFPointer)malloc(sizeof(MDFNode)); 
  mdfList->next = NULL; 
  afdList = (AFDPointer)malloc(sizeof(AFDNode)); 
  afdList->next = NULL; 
}

void 
lsuser() {
  MDFPointer cursor = mdfList->next;
  if (cursor == NULL) {
    printf("还没有用户，请创建用户\n"); 
    return ; 
  } else {
    printf("用户列表: \n"); 
  }
  while (cursor) {
    printf("\t%s\n", cursor->userName); 
    cursor = cursor->next; 
  }
}

UFDPointer  
findDirectoryByUserName(const char *userName) {
  MDFPointer cursor = mdfList->next; 
  while (cursor) {
    if (!strcmp(cursor->userName, userName)) {
      currentMDF = cursor; 
      return cursor->directoryPointer; 
    } 
    cursor = cursor->next; 
  }
  return NULL; 
}

int 
compareCmd(char *cmd1, 
    char *cmd2) {
  char *cursor1 = cmd1; 
  char *cursor2 = cmd2; 
  while(*cursor2 != '\0' && *cursor2 != '\n') {
    if (*cursor1 == *cursor2) {
      cursor1++; 
      cursor2++; 
      continue; 
    } else {
      return -1; 
    }
  }
  return 0; 
}

void 
changeString(char *string) {
  char *cursor = string; 
  while (*cursor != '\n') 
    cursor++; 
  *cursor = '\0'; 
}


void 
createUser() {
  char userName[NAMESIZE]; 
  printf("新建用户名: "); 
  /*scanf("%s", userName); fflush(stdin); */
  fgets(userName, NAMESIZE, stdin); changeString(userName); 
  MDFPointer newUser = (MDFPointer)malloc(sizeof(MDFNode)); 
  strcpy(newUser->userName, userName); 
  newUser->directoryPointer = (UFDPointer)malloc(sizeof(UFDNode)); 
  newUser->next = mdfList->next; 
  mdfList->next = newUser; 
  printf("用户创建成功\n"); 
}

void 
createFile() {
  /*char input[1024]; */
  UFDPointer newFile = (UFDPointer)malloc(sizeof(UFDNode)); 
  printf("新建文件[fileName length r w x]: "); 
  /*fgets(input, 1024, stdin); */
  fscanf(stdin, "%s%d%d%d%d", newFile->fileName, &(newFile->length), 
      &(newFile->canRead), &(newFile->canWrite), &(newFile->canExec)); 
  /*fflush(stdin); */
  while(!((newFile->canRead == 0 || newFile->canRead == 1) && 
      (newFile->canWrite == 0 || newFile->canWrite == 1) && 
      (newFile->canExec == 0 || newFile->canExec == 1))) {
    printf("输入错误，rwx只能是0或者1\n"); 
    printf("新建文件[fileName length r w x]: "); 
    fscanf(stdin, "%s%d%d%d%d", newFile->fileName, &(newFile->length), 
        &(newFile->canRead), &(newFile->canWrite), &(newFile->canExec)); 
  }
  getchar(); 
  newFile->next = directoryPointer->next; 
  directoryPointer->next = newFile; 
  printf("文件创建成功\n"); 
}


void 
lsfile() {
  UFDPointer cursor = directoryPointer->next; 
  if (!cursor) {
    printf("该用户还没有创建文件，请先创建文件\n"); 
    return ; 
  }
  while (cursor) {
    printf("%s\t%d\t%c%c%c\n", cursor->fileName, 
        cursor->length, 
        (cursor->canRead == 1) ? 'r' : '-', 
        (cursor->canWrite == 1) ? 'w' : '-', 
        (cursor->canExec == 1) ? 'x' : '-'); 
    /*printf("%s %d %s%s%s", cursor->fileName, */
        /*(cursor->canRead == 1) ? "r" : "-", */
        /*(cursor->canWrite == 1) ? "w" : "-", */
        /*(cursor->canExec == 1) ? "x" : "-"); */
    cursor = cursor->next; 
  }
}

void 
deleteFile() {
  char fileName[NAMESIZE]; 
  printf("请输入文件名: "); 
  fgets(fileName, NAMESIZE, stdin); changeString(fileName); 
  UFDPointer cursorPrev = directoryPointer; 
  UFDPointer cursor = cursorPrev->next; 
  while(cursor) {
    if (!strcmp(fileName, cursor->fileName)) {
      cursorPrev->next = cursor->next; 
      free(cursor); 
      printf("删除成功\n"); 
      return ; 
    }
    cursorPrev = cursor; 
    cursor = cursor->next; 
  }
  printf("没有找到该文件\n"); 
}

void 
lsopened() {
  AFDPointer cursor = afdList->next; 
  if (!cursor) {
    printf("没有文件被打开\n"); 
    return ; 
  }
  while(cursor) {
    printf("%s\t%s\t%d\n", cursor->fileName, cursor->userName, cursor->filePos); 
    cursor = cursor->next; 
  }
}

void 
openFile() {
  char fileName[NAMESIZE]; 
  printf("请输入文件名: "); 
  fgets(fileName, NAMESIZE, stdin); changeString(fileName); 
  UFDPointer cursor = directoryPointer->next; 
  while(cursor) {
    if (!strcmp(fileName, cursor->fileName)) {
      if (cursor->canRead && cursor->canExec) {
        AFDPointer newOpenFile = (AFDPointer)malloc(sizeof(AFDNode)); 
        strcpy(newOpenFile->fileName, fileName); 
        strcpy(newOpenFile->userName, currentMDF->userName); 
        newOpenFile->filePos = 0; 
        newOpenFile->ufdPointer = cursor; 
        newOpenFile->next = afdList->next; 
        afdList->next = newOpenFile; 
        printf("打开成功\n"); 
      } else {
        printf("该文件不能被打开，请检查rwx权限\n"); 
      } 
      return ; 
    }
    cursor = cursor->next; 
  }
  printf("没有找到该文件\n"); 
}

void 
closeFile() {
  char fileName[NAMESIZE]; 
  printf("请输入文件名: "); 
  fgets(fileName, NAMESIZE, stdin); changeString(fileName); 
  AFDPointer cursorPrev = afdList; 
  AFDPointer cursor = cursorPrev->next; 
  while(cursor) {
    if (strcmp(cursor->userName, currentMDF->userName) == 0 && 
        strcmp(cursor->fileName, fileName) == 0) {
      cursorPrev->next = cursor->next; 
      free(cursor); 
      printf("文件关闭成功\n"); 
      return ; 
    }
    cursorPrev = cursor; 
    cursor = cursor->next; 
  } 
  printf("该用户没有打开该文件\n"); 
}


void 
readFile() {
  char fileName[NAMESIZE]; 
  printf("请输入文件名: "); 
  fgets(fileName, NAMESIZE, stdin); changeString(fileName); 
  AFDPointer cursor = afdList->next; 
  while(cursor) {
    if (strcmp(cursor->userName, currentMDF->userName) == 0 && 
        strcmp(cursor->fileName, fileName) == 0) {
      cursor->filePos = cursor->ufdPointer->length - 1; 
      printf("读文件成功，文件指针已移到文件尾部\n"); 
      return ; 
    }
    cursor = cursor->next; 
  } 
  printf("该文件没有被打开\n"); 
}


void 
writeFile() {
  char fileName[NAMESIZE]; 
  printf("请输入文件名: "); 
  fgets(fileName, NAMESIZE, stdin); changeString(fileName); 
  AFDPointer cursor = afdList->next; 
  while(cursor) {
    if (strcmp(cursor->userName, currentMDF->userName) == 0 && 
        strcmp(cursor->fileName, fileName) == 0) {
      cursor->filePos = cursor->ufdPointer->length - 1; 
      printf("写文件成功，文件指针已移到文件尾部\n"); 
      return ; 
    }
    cursor = cursor->next; 
  } 
  printf("该文件没有被打开\n"); 
}

void 
working() {
  char userName[NAMESIZE]; 
  char cmd[CMDSIZE]; 
  int flag = 0; 
  printf("请输入用户名: ");
  fgets(userName, NAMESIZE, stdin); changeString(userName); 
  directoryPointer = findDirectoryByUserName(userName); 
  if (!directoryPointer) {
    printf("用户登入出错\n"); 
  } else {
    printf("[%s]$ ", userName); 
    fgets(cmd, CMDSIZE, stdin); 
    while (!compareCmd(cmd, "exit") || !compareCmd(cmd, "create") 
        || !compareCmd(cmd, "delete") || !compareCmd(cmd, "open") 
        || !compareCmd(cmd, "read") || !compareCmd(cmd, "write") 
        || !compareCmd(cmd, "lsfile") || !compareCmd(cmd, "lsopened") 
        || !compareCmd(cmd, "close") || !compareCmd(cmd, "help") 
        || !printf("不能执行该命令，输入help查看\n[%s]$ ", userName) 
        || !fgets(cmd, CMDSIZE, stdin) || (flag = 1)) {
      if (flag == 1) {
        flag = 0; 
        continue; 
      }
      if (!compareCmd(cmd, "exit")) {
        break; 
      } else if (!compareCmd(cmd, "create")) {
        createFile();        
      } else if (!compareCmd(cmd, "delete")) {
        deleteFile(); 
      } else if (!compareCmd(cmd, "open")) {
        openFile(); 
      } else if (!compareCmd(cmd, "close")) {
        closeFile(); 
      } else if (!compareCmd(cmd, "read")) {
        readFile(); 
      } else if (!compareCmd(cmd, "write")) {
        writeFile(); 
      } else if (!compareCmd(cmd, "lsfile")) {
        lsfile(); 
      } else if (!compareCmd(cmd, "lsopened")) {
        lsopened(); 
      } else if (!compareCmd(cmd, "help")) {
        printCmds(); 
      }

      printf("[%s]$ ", userName); 
      /*fflush(stdin); */
      fgets(cmd, CMDSIZE, stdin); 
    }

  }
}

int 
main(int argc, 
    char **argv) {
  init(); 
  printCmds();  

  /*char userName[NAMESIZE]; */
  char *cmd = (char *)malloc(CMDSIZE * sizeof(char)); 
  int flag = 0; 
  printf("$ "); 
  /*scanf("%s", cmd); fflush(stdin); */
  fgets(cmd, CMDSIZE, stdin); 
  /*fputs(cmd, stdin); */
  /*printf("%s", cmd); */
  /*printf("%d\n", strcmp(cmd, "help")); */
  /*printf("%d\n", compareCmd(cmd, "help")); */
  /*while (!strcmp(cmd, "exit") || !strcmp(cmd, "help") || */
      /*!strcmp(cmd, "new") || !strcmp(cmd, "use") || !strcmp(cmd, "lsuser") || */
      /*!printf("$ 不能执行该命令，输入help查看\n$ ") ||*/
      /*!scanf("%s", cmd) || fflush(stdin) || (flag = 1)) {*/
  while (!compareCmd(cmd, "exit") || !compareCmd(cmd, "help") 
      || !compareCmd(cmd, "new") || !compareCmd(cmd, "use")  
      || !compareCmd(cmd, "lsuser") || !compareCmd(cmd, "lsopened") 
      || !printf("$ 不能执行该命令，输入help查看\n$ ") 
      || !fgets(cmd, CMDSIZE, stdin) || (flag = 1)) { 
    if (flag) {
      flag = 0; 
      continue; 
    }
    if (!compareCmd(cmd, "exit")) {
      break; 
    } else if (!compareCmd(cmd, "help")) {
      printCmds(); 
    } else if (!compareCmd(cmd, "new")) {
      createUser(); 
    } else if (!compareCmd(cmd, "use")) {
      working(); 
    } else if (!compareCmd(cmd, "lsuser")) {
      lsuser(); 
    } else if (!compareCmd(cmd, "lsopened")) {
      lsopened(); 
    }

    printf("$ "); 
    /*scanf("%s", cmd); fflush(stdin); */
    fgets(cmd, CMDSIZE, stdin); 
  }


  return EXIT_SUCCESS; 
}
