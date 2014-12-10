	# long win_req(char* name)
	.global win_req
win_req:
	mov $500,%eax
	mov 4(%esp),%ecx
	mov $0,%edx
	int $100
	ret

# long win_color(int color)
	.global win_color
win_color:
	mov $501,%eax
	mov 4(%esp),%ecx
	mov $0,%edx
	int $100
	ret

# long win_cursor(int r, int c)
  .global win_cursor
win_cursor:
	mov $502,%eax
	mov 4(%esp),%ecx
	mov 8(%esp),%edx
	int $100
	ret

# long win_clear() 
  .global win_clear
win_clear:
  mov $503,%eax
  mov $0,%ecx
  mov $0,%edx
  int $100
  ret
