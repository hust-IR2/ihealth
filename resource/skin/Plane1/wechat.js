var startdiv=document.getElementById('startdiv');
var maindiv=document.getElementById('maindiv');
var score=document.getElementById('score');
var totalscore=document.getElementById('totalscore');
var enddiv=document.getElementById('enddiv');
var tid;
var scores=0;
var positionY=0;
//开始游戏按钮
window.setgameover = 2;
window.getGameOver = function (){
    return window.setgameover
} ;
window.setStart = 0;
window.getStartGame = function (){
    return window.setStart
}

var Checkint = 120;
if(true){
	function Checkpoint1(){
		Checkint = 30;
	}
	function Checkpoint2(){
		Checkint = 60
	}
	function Checkpoint3(){
		Checkint = 100
	}
}

function begin(){
	startdiv.style.display='none';
	maindiv.style.display='block';
	window.setgameover = 0;
    window.setStart = 1;
	tid=setInterval(start,Checkint);
}

//重新开始
function restart(){
	window.location.reload(true);
}
//-------------------------创建对象--------------------------------
//创建父类构造函数
function plane(x,y,imgsrc,speed,w,h,boomsrc,hp,dietime,score){
	this.x=x;
	this.y=y;
	this.imgsrc=imgsrc;
	this.speed=speed;
	this.w=w;
	this.h=h;
	this.boomsrc=boomsrc;
	this.hp=hp;
	//是否死亡
	this.planeisdie=false;
	//延时爆炸
	this.dietime=dietime;
  	this.diecount = 0;
  	this.score=score;



//显示飞机原理:创建节点,插入节点
	this.planenode=null;
	this.init=function(){
		this.planenode=document.createElement("img");
		this.planenode.style.position='absolute';
		this.planenode.style.left=this.x+'px';
		this.planenode.style.top=this.y+'px';
		this.planenode.src=this.imgsrc;
	};
	this.init();

	//插入接点
	maindiv.appendChild(this.planenode);
	//敌方飞机移动
	this.move=function(){
		this.planenode.style.top=this.planenode.offsetTop+this.speed+'px';
	}
}
//--------------------我方飞机--------------------
//创建本方飞机的构造函数
// function selfplane(x,y){
// 	plane.call(this,x,y,"image/我的飞机.gif");
// }
//创建本方飞机对象
var selfplane=new plane((document.documentElement.clientWidth/2)-36,580,"image/my_enemy.gif",null,null,null,null,null,null,null);
//本方飞机移动
var selfmove=function(x,y){
	//var e=window.event||arguments[0];
	//var selfplaneX=e.clientX;
	//var selfplaneY=e.clientY;
	//var x=e.clientX;
	//var y=e.clientY;
	 //本方飞机移动不超过maindiv边界
	//if(!(x<533||x>787||y<40||y>528)) {
		selfplane.planenode.style.left=x+'px';
		//selfplane.planenode.style.top=y+'px';
	//}
};

function getWidth() {
	var x = selfplane.x*2;
	return x
};
function getHeight() {
	return selfplane.y
};


function getScore() {
	return scores;
}


function getGameType() {
	return '飞机大战中级'
}


function getNandu() {
	if (Checkint <= 30) {
		return '高';
	}
	
	if (Checkint <= 60) {
		return '中';
	}
	
	return '低';
}

getWidth()

//-------------------敌方飞机--------------------
// 创建敌方飞机构造函数
function enemy(x,y,imgsrc,speed,w,h,boomsrc,hp,dietime,score){
	plane.call(this,random(0,document.documentElement.clientWidth),0,imgsrc,speed,w,h,boomsrc,hp,dietime,score);
}
function random(x,y){
	return Math.floor(Math.random()*(y-x)+x);
}
// 事件监听
if(document.addEventListener){
	maindiv.addEventListener('mousemove',selfmove,true);
}else if (document.attachEvent) {
	maindiv.attachEvent('mousemove',selfmove);
}
//-----------------------子弹------------------------
//创建子弹的构造函数
function  bullet(x,y,imgsrc){
	this.x=x;
	this.y=y;
	this.imgsrc=imgsrc;

	//创建子弹节点
	this.bulletnode=null;
	this.init=function(){
		this.bulletnode=document.createElement('img');
		this.bulletnode.style.position='absolute';
		this.bulletnode.style.left=this.x+'px';
		this.bulletnode.style.top=this.y+'px';
		this.bulletnode.src=this.imgsrc;
		maindiv.appendChild(this.bulletnode);
	}
	this.init();

	//子弹移动
	this.move=function(){
		this.bulletnode.style.top=this.bulletnode.offsetTop-20+'px';
	}

}
//创建单行子弹
function oddbullet(x,y){
	bullet.call(this,x,y,'image/bullet1.png');
}


//循环调用
//敌方飞机
var enemies=[];
var bullets=[];
var time1=0;
var time2=0;
function fire(){
  //这样可以调用数据
		bullets.push(new oddbullet(selfplane.planenode.offsetLeft + 32, selfplane.planenode.offsetTop));

}

function start(){

	var clientWidth = 0;
	if(true){
		clientWidth = document.documentElement.clientWidth
	}
	maindiv.style.backgroundPositionY=positionY+'px';
	positionY+=0.9;
	if(positionY==568){
		positionY=0;
	}
	time1++;
	if(time1==100){
		time2++;
		if (time2%5==0) {
			enemies.push(new enemy(25,clientWidth,'image/enemy3_fly_1.png',1,46,64,'image/enemy3_fly_1_boom.gif',5,400,500));
		}
		if (time2==20) {
			enemies.push(new enemy(57,clientWidth,'image/enemy2_fly_1.png',1,110,170,'image/enemy2_fly_1_boom.gif',20,600,1000));
			time2=0;
		}else{
			enemies.push(new enemy(5,clientWidth,'image/enemy1_fly_1.png',1,34,24,'image/enemy1_fly_1_boom.gif',1,200,10));
		}
		time1=0;
	}
	 //遍历数组,调用move函数
  	var enemylen = enemies.length;
  	for (var i = 0; i < enemylen; i++) {
    	enemies[i].move();
    	//飞机掉落之后要移除
    	if(enemies[i].planenode.offsetTop>768){
    	maindiv.removeChild(enemies[i].planenode);//删除节点
    	enemies.splice(i,1);//删除数组中的元素
    	enemylen--;
    	}
    	//判断当前飞机是否为死亡
	    if(enemies[i].planeisdie == true) {
	      enemies[i].diecount += 20;
	      if (enemies[i].diecount == enemies[i].dietime) {
	        //清除飞机
	        maindiv.removeChild(enemies[i].planenode);
	        enemies.splice(i,1);
	        enemylen--;
	      }
	    }
	    	//本方飞机与地方飞机碰撞
			if (enemies[i].planeisdie == false) {
				if (enemies[i].planenode.offsetLeft <= selfplane.planenode.offsetLeft + 66 && enemies[i].planenode.offsetLeft + enemies[i].w >= selfplane.planenode.offsetLeft) {
					if (enemies[i].planenode.offsetTop <= selfplane.planenode.offsetTop + 80 && enemies[i].planenode.offsetTop + enemies[i].h >= selfplane.planenode.offsetTop) {
						//碰撞
                        window.setgameover = 1;
						//切换本方图片
						selfplane.planenode.src = 'image/my_enemyboom.gif';
						//显示结束界面
						enddiv.style.display = 'block';
						//显示分数
						totalscore.innerHTML = scores;
						//结束循环
						clearInterval(tid);
						//取消鼠标监听事件
						if (document.removeEventListener) {
							maindiv.removeEventListener('mousemove', selfmove, true);
						} else if (document.detachEvent) {
							maindiv.detachEvent('onmousemove', selfmove);
						}
						break;
					}
				}
			}
    }

    if(bullets.length>0) {
        //创建子弹

        //移动子弹
        var bulletlen = bullets.length;
        for (var i = 0; i < bulletlen; i++) {
            bullets[i].move();
            //移除子弹
            if (bullets[i].bulletnode.offsetTop < 0) {
                maindiv.removeChild(bullets[i].bulletnode);//删除节点
                bullets.splice(i, 1);//删除数组中的元素
                bulletlen--;
            }
        }

        //碰撞判断
        for (var k = 0; k < bulletlen; k++) {
            for (var j = 0; j < enemylen; j++) {
                //子弹与敌机碰撞
                if (bullets[k].bulletnode.offsetLeft <= enemies[j].planenode.offsetLeft + enemies[j].w && bullets[k].bulletnode.offsetLeft + 6 >= enemies[j].planenode.offsetLeft) {
                    if (bullets[k].bulletnode.offsetTop <= enemies[j].planenode.offsetTop + enemies[j].h && bullets[k].bulletnode.offsetTop + 14 >= enemies[j].planenode.offsetTop) {
                        //四个条件同时满足,才会碰撞
                        enemies[j].hp -= 1;
                        if (enemies[j].hp == 0) {
                            enemies[j].planenode.src = enemies[j].boomsrc;

                            //飞机标记为死亡
                            enemies[j].planeisdie = true;

                            //统计分数
                            scores += enemies[j].score;
                            score.innerHTML = scores;
                        }

                        //删除子弹
                        maindiv.removeChild(bullets[k].bulletnode);
                        bullets.splice(k, 1);
                        bulletlen--;
                    }
                }
            }
        }
    }
}





