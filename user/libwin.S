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
