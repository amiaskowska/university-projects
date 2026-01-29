section .text

; Zwraca i-ty bit w tablicy t.
%macro GETBIT 3 
    ; %1 -> rejestr docelowy.
    ; %2 -> t* - wskaźnik do tablicy.
    ; %3 -> i - indeks bitu.
    mov     rbx, rcx 
    mov     r11, %3             
    mov     rcx, r11
    shr     r11, 6              ; r11 = i / 64 (numer słowa).
    and     rcx, 63             ; rcx = i mod 64 (numer bitu w słowie).
    bt      [%2 + r11 * 8], rcx ; Bit test.
    setc    %1                  ; Ustawia odpowiednią wartość w rejestrze.
    mov     rcx, rbx            ; Przywraca rcx. 

%endmacro

; Zamienia i-ty bit w tablicy t na przeciwny.
%macro BITFLIP 2  
    ; %1 -> t* - wskaźnik do tablicy.
    ; %2 -> i - indeks bitu.
    mov     rbx, rcx 
    mov     r11, %2             
    mov     rcx, r11
    shr     r11, 6              ; r11 = i / 64 (numer słowa).
    and     rcx, 63             ; rcx = i mod 64 (numer bitu w słowie).
    btc     qword [%1 + r11 * 8], rcx 
    mov     rcx, rbx            ; Przywraca rcx.
    
%endmacro

; Ustawia i-ty bit w tablicy t na 1.
%macro BITON 2 
    ; %1 -> t* - wskaźnik do tablicy.
    ; %2 -> i - indeks bitu.
    mov     rbx, rcx 
    mov     r11, %2             
    mov     rcx, r11
    shr     r11, 6              ; r11 = i / 64 (numer słowa).
    and     rcx, 63             ; rcx = i mod 64 (numer bitu w słowie).
    bts     qword [%1 + r11 * 8], rcx 
    mov     rcx, rbx            ; Przywraca rcx.


%endmacro

; Ustawia i-ty bit w tablicy t na 0.
%macro BITOFF 2 
    ; %1 -> t* - wskaźnik do tablicy.
    ; %2 -> i - indeks bitu.
    mov     rbx, rcx 
    mov     r11, %2           
    mov     rcx, r11
    shr     r11, 6              ; r11 = i / 64 (numer słowa).
    and     rcx, 63             ; rcx = i mod 64 (numer bitu w słowie).
    btr     qword [%1 + r11 * 8], rcx 
    mov     rcx, rbx            ; Przywraca rcx.
%endmacro

; r8 -> isGreater - czy dzielnik jest większy niż obecny fragment dzielnej. 
; r9 -> bit - Iterator po bitach x'a (dzielnika).
; r10 -> i - Iterator po bitach dzielnej.
; r12 -> msb - Najbardziej znaczący bit w x.
; r13 -> left - Lewy koniec rozpatrywanego przedziału dzielnej.
; r14 -> right - Prawy koniec rozpatrywanego przedziału dzielnej.
; r15 -> Wskaźnik na y.

; Sprawdza, czy dzielnik jest większy niż obecny fragment dzielnej.
%macro comparison 1 
    ; %1 -> Czy to pierwsze wywołanie porównania.
    mov     r9, r14             
    sub     r9, r13             ; bit = right - left
    xor     r8, r8              ; isGreater = 0
    mov     r10, r14            ; i = right

%%compare_loop:
    cmp     r10, r13            ; while (i >= left) 
    jl      %%end_compare_loop

    %if %1 == 1                 ; Pierwsze porównanie.
        mov     rax, rdx        ; rax = n
        cmp     r10, rax        
        jne     %%get_bit1      ; Jeśli i != n, to bit1 pobrany z y.
        mov     bl, 1           ; Jeśli i == n, to bit1 = 1, niezapisany w y.
        jmp     %%get_bit2
    %endif
%%get_bit1:
    GETBIT  bl, r15, r10        ; bl = bit1 = GETBIT(y, i)
%%get_bit2:

    cmp     r9, r12             
    jg      %%bit2_zero         ; Jeśli bit > k, to bit2 = 0 (zera wiodące).
    GETBIT  bh, rsi, r9         ; Jeśli bit <= k, to bit2 = x[bit]
    jmp     %%got_bit2
%%bit2_zero:
    xor     bh, bh              ; bit2 = 0 (zera wiodące).
%%got_bit2:
    dec     r9                  ; bit--

    cmp     bl, 1               ; Jeśli bit1 == 1
    jne     %%check_less
    cmp     bh, 0               ; Oraz jeśli bit2 == 0
    jne     %%check_less
    jmp     %%end_compare_loop  ; To dzielna > dzielnik - break.

%%check_less:
    cmp     bl, 0               ; Jeśli bit1 == 0
    jne     %%next_iter
    cmp     bh, 1               ; Oraz jeśli bit2 == 1
    jne     %%next_iter
    mov     r8b, 1              ; isGreater = true
    jmp     %%end_compare_loop  ; Dzielna < dzielnik. 

%%next_iter:
    dec     r10                 ; i--
    jmp     %%compare_loop

%%end_compare_loop:
%endmacro


; r8 -> borrow - Czy pożyczono w odejmowaniu. 
; r9 -> bit - Iterator po bitach x (dzielnika).
; r10 -> i - Iterator po bitach dzielnej.
; r12 -> msb - Najbardziej znaczący niezerowy bit w x.
; r13 -> left - Lewy koniec rozpatrywanego przedziału dzielnej.
; r14 -> right - Prawy koniec rozpatrywanego przedziału dzielnej.
%macro substraction 0
    xor     r9, r9              ; bit = 0
    xor     r8, r8              ; borrow = 0
    mov     r10, r13            ; i = left

%%substract_loop:
    cmp     r10, r14            ; while i <= right 
    jg      %%end_subtract_loop

    mov     rax, rdx            ; rax = n
    cmp     r10, rax            
    jne     %%get_bit1_sub      ; Jeśli i != n, to bit1 pobrany z y.
    mov     bl, 1               ; Jeśli i== n, to bit1 = 1, niezapisany w y.
    jmp     %%get_bit2_sub
%%get_bit1_sub:
    GETBIT  bl, r15, r10        ; bl = bit1 = GETBIT(y, i)
%%get_bit2_sub:
    mov     cl, bl              ; cl -> Niezmieniony bit1.

    cmp     r9, r12             
    jg      %%bit2_is_zero_sub  ; Jeśli bit > k, to bit2 = 0 (zera wiodące).
    GETBIT  bh, rsi, r9         ; Jeśli bit <= k, to bit2 pobierz z x.
    jmp     %%got_bit2_sub
%%bit2_is_zero_sub:
    xor     bh, bh              ; bit2 = 0.
%%got_bit2_sub:

    cmp     r8b, 1              ; Czy borrow == true?
    jne     %%after_borrow      ; borrow == false.
    BITFLIP r15, r10            ; borrow == true, to zmiana bitu dzielnej na przeciwny.
    cmp     cl, 1               ; Czy bit1 == 1?
    jne     %%borrow_remains    ; Jeśli bit1 != 1, to borrow zostaje bez zmian.
    mov     r8b, 0              ; Jeśli bit1 == 1, to borrow = false.
%%borrow_remains:
    xor     bl, 1               ; bit1 zmienił się na przeciwny po BITFLIP.

%%after_borrow:
    cmp     bh, 1               
    jne     %%next_iter_sub     ; bit2 == 0, czyli bit się nie zmienia.
    cmp     bl, 0               
    jne     %%no_new_borrow     ; bit1 == 1, czyli bez pożyczania.
    mov     r8b, 1              ; bit2 == 0, więc borrow = true.
%%no_new_borrow:
    BITFLIP r15, r10            ; Odejmownie (zmiana na przeciwny bit).

%%next_iter_sub:
    inc     r9                  ; bit++
    inc     r10                 ; i++
    jmp     %%substract_loop

%%end_subtract_loop:
%endmacro

global ninv
ninv:        
    push    rbx
    push    r12
    push    r13
    push    r14
    push    r15

    mov     r15, rdi            ; r15 = y*
    ; r12 -> k - Najbardziej znaczący bit w x.
    ; r13 -> left - Lewy koniec rozpatrywanego przedziału dzielnej.
    ; r14 -> right - Prawy koniec rozpatrywanego przedziału dzielnej.
    ; rax, rcx, r8-r11, rbx - rejestry na zmienne pomocniczne. 

    ; Zerowanie tablicy y - teraz przechowuje 2^n bez (n+1)-szego bitu.
    mov     rcx, rdx
    shr     rcx, 6              ; rcx = n / 64
    xor     rax, rax            ; rax = 0, do zapisania w tablicy.
    rep     stosq               ; Wyzerowanie tablicy y. 

    ; Zlicza bity w x i znajduje największy - czy x to potęga dwójki?
    xor     r8, r8              ; r8 -> k = 0 (indeks największego bitu + 1).
    xor     r9, r9              ; r9 -> c = 0 (licznik zapalonych bitów).
    xor     r10, r10            ; r10 -> i = 0 (iterator).

.count_bits_loop:
    cmp     r10, rdx            ; while (i < n)
    jge     .end_count_bits
    
    GETBIT  al, rsi, r10        ; al = x[i]  
    
    cmp     al, 1               
    jne     .next_count_bit     ; x[i] == 0, kontynuuj.
    
    mov     r8, r10             ; k = i + 1
    inc     r8
    inc     r9                  ; Znaleziono niezerowy bit, c++.

.next_count_bit:
    inc     r10                 ; i++
    jmp     .count_bits_loop

.end_count_bits:
    cmp     r9, 1               ; Czy jest tylko jeden zapalony bit w x?
    jne     .find_msb_x         ; Jeśli nie, kontynuuj standardowy algorytm.
    
    ; Tylko jeden zapalony bit w x - dzielnik jest potęgą dwójki.
    mov     rax, rdx       
    sub     rax, r8              
    inc     rax                 ; rax = n - k + 1
    BITON   r15, rax            ; Ustaw jedyny bit wynikowy.
    jmp     .end_function      

.find_msb_x:
    ; Wyznacza najbardziej znaczący bit w x - msb.
    xor     r12, r12            ; msb = 0
    mov     r8, rdx         
    shr     r8, 6               
    dec     r8                  ; r8 -> i = (n/64) - 1
.find_msb_loop:
    cmp     r8, 0               ; while (i >= 0)
    jl      .found_msb
    mov     rax, [rsi + r8*8]   ; rax = x[i]
    cmp     rax, 0              
    je      .find_msb_next      ; Jeśli x[i] == 0, przejdź dalej.
    bsr     r9, rax             ; r9 -> j = najbardziej znaczący bit z x[i].
    mov     r12, r8
    shl     r12, 6              
    add     r12, r9             ; msb = 64 * i + j 
    jmp     .found_msb
.find_msb_next:
    dec     r8                  ; i--
    jmp     .find_msb_loop
.found_msb:

    mov     rax, rdx            ; rax -> n
    mov     r14, rax            ; r14 -> right = n - prawy koniec przedziału.
    sub     rax, r12            
    mov     r13, rax            ; r13 -> left = n - msb - lewy koniec przedziału.

    ; Sprawdza, czy x mieści się na sufiksie swojej długości.
    comparison 1                ; Pierwsze wywołanie.

    cmp     r8b, 1              ; Czy x jest większe niż czytany fragment dzielnej?
    jne     .after_cmp          ; Nie, na tym przedziale wynikiem dzielenia jest 1.
    dec     r13                 ; Tak, rozważany będzie dluższy przedział, left--.
.after_cmp:

    ; Odjęcie dzielnika na przedziale.
    substraction

.main_loop:
	; Główna pętla wykonująca kolejne kroki dzielenia pisemnego. 
    cmp     r13, 0              ; while (left > 0)
    jle     .end_main_loop
    dec     r13                 ; left--
    dec     r14                 ; right--

    ; Porównaj dzielnik z czytanym fragmentem dzielnej.
    comparison 0       

    cmp     r8b, 0              ; Czy dzielnik jest niewiększy niż dzielna?
    jne     .result_zero        ; Jest większy - bit wynikowy to 0.

.result_one:
    ; Bit wynikowy to 1.
    substraction
    BITON   r15, r14            ; Ustaw bit wynikowy (right) na 1 w tablicy y.
    jmp     .next_iteration

.result_zero:
    ; Bit wynikowy to 0.
    BITOFF  r15, r14            ; Ustaw bit wynikowy (right) na 0 w tablicy y.

.next_iteration:
    jmp     .main_loop
.end_main_loop:

    xor     r8, r8              ; r8 -> i - Iterator po bitach.
.shift_loop:
    ; Poprawia odpowiednie umiejscowienie wyniku w tablicy y.

    ; Bity przesuwane są do indeksu 0. 
    mov     rax, r8             
    add     rax, r14            ; rax = i + right
    cmp     rax, rdx            ; while ((i + right) < n)
    jge     .end_shift_loop
    GETBIT  bl, r15, r8         ; bl -> bit1 = y[i]
    mov     rax, r8             
    add     rax, r14
    GETBIT  bh, r15, rax        ; bh -> bit2 = y[i + right]
    cmp     bl, bh              ; Czy obecny bit jest poprawny?
    je      .no_flip            ; Jeśli tak, to zostaje taki sam.
    BITFLIP r15, r8             ; Jeśli nie, to zamień na przeciwny.
.no_flip:
    inc     r8                  ; i++
    jmp     .shift_loop
.end_shift_loop:

    ; Wstawia jedynkę z pierwszego dzielenia, która nie była zapisana w y.
    mov     rax, rdx
    sub     rax, r14            ; rax = n - right 
    BITON   r15, rax            ; Ustawia najbardziej znaczący bit na 1.

    ; Pozostałe bity zostają wyzerowane.
    mov     r8, rdx        
    sub     r8, r14             
    inc     r8                  ; i = n - right + 1
.zero_loop:
    cmp     r8, rdx             ; while (i < n)
    jge     .end_zero_loop
    BITOFF  r15, r8             ; y[i] = 0
    inc     r8                  ; i++
    jmp     .zero_loop
.end_zero_loop:

.end_function:
    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     rbx
    ret
