#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>


using std::vector ;

#define ROW 10
#define COLUMN 10


typedef struct stPoint
{
	stPoint() : x(0),y(0),px(0),py(0),f(0),g(0),h(0)
	{}

	int x;
	int y;
	int px; /* 父结点横坐标 */
	int py; /* 父结点纵坐标 */
	int f;
	int g;
	int h;
}POINT;

//
// ↑ ↓ ← → ↖ ↗ ↙ ↘
//

void calcOri( POINT p1, POINT p2 , std::string & ch)
{
	if( abs(p1.x - p2.x) + abs(p1.y - p2.y) == 1 )
	{
		if(p2.x - p1.x > 0)
		{
			ch = "↓";
		}
		else
		{
			ch = "↑";
		}

		if(p2.y - p1.y > 0)
		{
			ch = "→" ;
		}
		else
		{
			ch = "←";
		}
	}
	else
	{
		if(p2.x < p1.x)
		{
			if(p2.y > p1.y) 
			{
				ch = "↗" ;
			}
			else
			{
				ch = "↖";
			}
		}
		else
		{
			if(p2.y > p1.y) 
			{
				ch = "↘" ;
			}
			else
			{
				ch = "↙";
			}
		}
	}

}




/*
 * 地图0为通路，1为起点，2为终点，-1为不可达
 */
 

int short_map[][COLUMN] = 
{{ 0, 0, 0, -1, 0 , 0, 0, 0, 0, 0		},
{ 0, -1, 0, -1, 0 , 0, 0, -1, 0, 0		},
{1, -1, 0, 0, 0 , 0, -1, 0, -1, 0		},
{0, 0, 0, -1, -1 , 0, 0, 0, -1, 0		},
{0, -1, 0, 0, 0 , 0, 0, -1, 0, 0		},
{0, 0, 0, -1, -1 , 0, 0, 0, -1, 0		},
{0, -1, 0, 0, 0 , 0, 0, -1, 0, 0		},
{0, 0, 0, -1, -1 , 0, 0, 0, -1, 0		},
{0, -1, 0, 0, 0 , 0, 0, -1, 0, 2		},
{0, 0, 0, -1, -1 , 0, 0, 0, -1, 0		},
};

vector<POINT> close_map;
vector<POINT> open_map;

void InsertOpenList(POINT& p)
{
	open_map.push_back(p);
}

void InsertCloseList(POINT& p)
{
	close_map.push_back(p);
}

void calcBound(POINT cur, int& sX, int & sY ,int & eX,int & eY)
{
	sX = (cur.x > 0) ? (cur.x - 1 ) : cur.x ;
	sY = (cur.y > 0) ? (cur.y - 1 ) : cur.y ;

	eX = (cur.x < ROW)  ? cur.x + 1  : cur.x ;
	eY = (cur.y < COLUMN) ? cur.y + 1 : cur.y;
	
	//printf("%d , %d ,%d ,%d \n" , sX, sY , eX, eY);
}

void showList(vector<POINT> tt)
{
	vector<POINT>::iterator iter = tt.begin();
	printf("\nList\n");
	for(; iter != tt.end() ; iter ++ )
	{
		printf(" %d %d  " , iter->x , iter->y);
	}

	printf("\n");
}


POINT* IsInCloseList(int x, int y)
{
	vector<POINT>::iterator iter = close_map.begin();
	int pos;
	for( pos = 0; iter != close_map.end() ; iter ++ , pos ++)
	{
		if( x == iter->x && y == iter->y )
			return &(close_map[pos]);
	}

	return NULL;
}

POINT*  IsInOpenList(int x, int y)
{
	vector<POINT>::iterator iter = open_map.begin();
	int pos;
	for(pos = 0 ; iter != open_map.end() ; iter ++ , pos ++)
	{
		if( x == iter->x && y == iter->y )
			return &(open_map[pos]);
	}

	return NULL;
}

int getListCount(vector<POINT> pt) 
{
	return pt.size();
}

int  GetclosemapIndex(int x, int y)
{
	vector<POINT>::iterator iter = close_map.begin();
	int pos;
	for( pos = 0; iter != close_map.end() ; iter ++ , pos ++)
	{
		if( x == iter->x && y == iter->y )
			return pos;
	}
	return -1;
}



//	G : 从当前结点 移动到 下个结点 
int calcG(POINT cur , int nx,int ny)
{
	if(cur.x == nx || cur.y == ny)
	{
		return 10;
	}
	else
		return 14;
}
// H : 从指定结点 移动到 终点的估算成本
// 这里只考虑 横向纵向移动 不考虑 斜着移动

int calcH(POINT end , int nx,int ny)
{
	int stepNum = 0;
	stepNum += ((end.x >  nx) ? 1 : -1 ) * (end.x - nx );
	stepNum += ((end.y >  ny) ? 1 : -1 ) * (end.y - ny );

	return stepNum * 10;
}


void FindNext(POINT cur,POINT end,POINT &next)
{
	int startX = 0;		//初始化起始扫描 坐标
	int startY = 0;
	int endX = 0;			// 初始化终止扫描 坐标
	int endY = 0;

	InsertCloseList(cur);		//将当前位置加入 关闭列表

	//确定扫描边界
	calcBound(cur , startX ,startY , endX , endY); 

	POINT* pNext = NULL;
	//最小 f 值, 以及 最小花费坐标
	int min = 0;
	for ( int i = startX ; i <= endX ; i ++ )
	{
		for( int j = startY ; j <= endY ; j ++)
		{
			//printf(" i j : %d ,%d \n" , i , j );
			//showList(close_map);
			if(IsInCloseList( i ,j ))
			{
				//printf(" \n [%d %d]\n ", i ,j );
				// 已经在关闭列表 或者此路不通则继续扫描
				//printf("true");
				continue;
			}

			if(-1 == short_map[i][j])
			{
					continue;
			}

			// 找到一个相邻结点, 判断是否已经在 开启列表
			
			POINT* pTemp = IsInOpenList(i , j ); 
			if( NULL == pTemp)	//不在开启列表 则加入开启列表
			{
				pTemp = new POINT();
				pTemp->x = i;
				pTemp->y = j;
				pTemp ->px = cur.x;
				pTemp ->py = cur.y;
				pTemp ->g = calcG(cur , i , j);
				pTemp ->h = calcH(end , i , j);
				pTemp ->f = pTemp->g + pTemp->h;
				InsertOpenList(*pTemp);
			}
			else
			{
				//	已经在开启列表 更新 g 值
				int g =calcG(cur , i , j);
				if( (g + cur.g) <  pTemp->g )
				{
					pTemp->px = cur.x;
					pTemp->py = cur.y;
					pTemp->g = g;
					pTemp ->f = pTemp->g + pTemp->h;
				}
			}

			int value = pTemp -> f;
			if( 0 == min || min > value)
			{
				min = value;
				pNext = pTemp;
			}

			//printf("\n %d , %d \n" ,pTemp->x , pTemp->y );
			
		}
	}

	next = * pNext;
}



void draw()
{
	int pos  = -1;
	for( int i = 0 ; i < ROW ; i ++)
	{
		for(int j = 0 ; j < COLUMN ; j++)
		{
			pos = GetclosemapIndex(i , j);
			if( pos == -1 || pos == getListCount(close_map)-1 )
			{
				printf("%2d", short_map[i][j]);
			}
			else
			{
				std::string ch;
				calcOri(close_map[pos], close_map[pos+1] ,ch);
				std::cout<<ch;
			}
				
		}
		printf("\n");
	}

}

void Astar()
{
	POINT start;
	POINT end;
	POINT cur;
	POINT next;

	// 查找开始和结束结点
	for( int i = 0 ; i < ROW ; i ++)
	{
		for(int j = 0 ; j < COLUMN ; j++)
		{
			if(1 == short_map[i][j])
			{
				start.x = i;
				start.y = j;
				cur.x = i ;
				cur.y = j;
			}
			if( 2 == short_map[i][j])
			{
				end.x = i;
				end.y = j;
			}
	//	printf("%2d ",short_map[i][j]);
		}
		printf("\n");
	}

	while(1)
	{
		FindNext(cur,end,next);
		if( next.x == end.x && next.y == end.y)
		{
			InsertCloseList(next);
			//showList(close_map);
			
			 draw();
			
		
			break;
		}
		cur = next;
	}
}




int _tmain(int argc, _TCHAR* argv[])
{
	Astar();
	return 0;
}

