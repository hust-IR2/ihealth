    //获得主界面
var mainDiv=document.getElementById("maindiv");
    //获得开始界面
var startdiv=document.getElementById("startdiv");
    //获得游戏中分数显示界面
var scorediv=document.getElementById("scorediv");
//获得游戏中鼠标的显示
var mousediv=document.getElementById("mouseXY");
//获得分鼠标位置
var mousep=document.getElementById("mousep");
    //获得分数界面
var scorelabel=document.getElementById("label");
    //获得暂停界面
var suspenddiv=document.getElementById("suspenddiv");
    //获得游戏结束界面
var enddiv=document.getElementById("enddiv");
    //获得游戏结束后分数统计界面
var planscore=document.getElementById("planscore");
//获得武器界面
var weapsDiv=document.getElementById("weaps");
//获得武器界面导弹列表id
var daodanlist=document.getElementById("daodanlist");
//获得武器界面导弹图片id
var daodanimg=document.getElementById("daodanimg");
//获得武器界面导弹数量
var numdaodan=document.getElementById("numdaodan");
    //初始化分数
var scores=0;

/*
创建飞机类
*/
function plan(hp,X,Y,sizeX,sizeY,score,dietime,sudu,imagesrc){
	  this.planX=X;
	    this.planY=Y;
	    this.imagenode=null;
	    this.planhp=hp;
	    this.planscore=score;
	    this.plansizeX=sizeX;
	    this.plansizeY=sizeY;
	   
	    this.planisdie=false;
	    this.plandietimes=0;
	    this.plandietime=dietime;
	    this.plansudu=sudu;
	    this.weaptype=0;
	   
	  //行为
	    /*
	    移动行为
	         */
	        this.planmove=function(){
	            if(scores<=80000){
	                this.imagenode.style.top=this.imagenode.offsetTop+this.plansudu+"px";
	                mainDiv.style.backgroundImage="url(image/bg2.jpg)";
	            }
	            else if(scores>80000&&scores<=290000){
	                this.imagenode.style.top=this.imagenode.offsetTop+this.plansudu+1+"px";
	                mainDiv.style.backgroundImage="url(image/bg1.jpg)";
	            }
	            else if(scores>190000&&scores<=360000){
	                this.imagenode.style.top=this.imagenode.offsetTop+this.plansudu+2+"px";
	                mainDiv.style.backgroundImage="url(image/bg3.jpg)";
	            }
	            else if(scores>260000&&scores<=450000){
	                this.imagenode.style.top=this.imagenode.offsetTop+this.plansudu+3+"px";
	                mainDiv.style.backgroundImage="url(image/bg4.jpg)";
	            }
	            else if(scores>350000&&scores<=550000){
	                this.imagenode.style.top=this.imagenode.offsetTop+this.plansudu+4+"px";
	                mainDiv.style.backgroundImage="url(image/bg4.jpg)";
	            }
	            else{
	                this.imagenode.style.top=this.imagenode.offsetTop+this.plansudu+5+"px";
	            }
	        }
	        this.init=function(){
	            this.imagenode=document.createElement("img");
	            this.imagenode.style.left=this.planX+"px";
	            this.imagenode.style.top=this.planY+"px";
	            this.imagenode.src=imagesrc;
	            mainDiv.appendChild(this.imagenode);
	           
	        }
	        this.init();
}

/*
创建本方飞机类
 */
function ourplan(X,Y){
	
    var imagesrc="image/own.png";
    plan.call(this,1,X,Y,66,80,0,660,0,imagesrc);
    this.imagenode.setAttribute('id','ourplan');
  
}
/*
创建本方飞机
*/
var selfplan=new ourplan(120,485);

//移动事件
var ourPlan=document.getElementById('ourplan');

var yidong=function(){
	 var oevent=window.event||arguments[0];
	    var chufa=oevent.srcElement||oevent.target;
//alert("ddddd");
    var selfplanX=oevent.clientX-500;
    var selfplanY=oevent.clientY;
    mousep.innerHTML="X:"+oevent.clientX+",Y:"+oevent.clientY;
    ourPlan.style.left=selfplanX-selfplan.plansizeX/2+"px";
    ourPlan.style.top=selfplanY-selfplan.plansizeY/2+"px";
//    document.getElementsByTagName('img')[0].style.left=selfplanX-selfplan.plansizeX/2+"px";
//    document.getElementsByTagName('img')[0]..style.top=selfplanY-selfplan.plansizeY/2+"px";
}
//判断本方飞机是否移出边界,如果移出边界,则取消mousemove事件,反之加上mousemove事件
var bianjie=function(){
	var oevent=window.event||arguments[0];
	var bodyobjX=oevent.clientX;
	var bodyobjY=oevent.clientY;
	if(bodyobjX<532||bodyobjX>788||bodyobjY<39||bodyobjY>527){
		
		if(document.removeEventListener){
			mainDiv.removeEventListener("mousemove",yidong,true);
		}else if(document.detachEvent){
			mainDiv.detachEvent("onmousemove",yidong);
		}
		
	}else{
		if(document.addEventListener){
			mainDiv.addEventListener("mousemove",yidong,true);
		}else if(document.attachEvent){
			mainDiv.attachEvent("onmousemove",yidong);
		}
	}
}

var bodyobj=document.getElementsByTagName("body")[0];

if(document.addEventListener){
	mainDiv.addEventListener("mousemove",yidong,true);
	bodyobj.addEventListener("mousemove",bianjie,true);
}else if(document.attachEvent){
	mainDiv.attachEvent("onmousemove",yidong);
	bodyobj.attachEvent("onmousemove",bianjie);
}

//初始化隐藏本方飞机
selfplan.imagenode.style.display="none";

//-----------------------------------------------------------------------------------子弹类
/*
创建子弹类
 */
function bullet(X,Y,sizeX,sizeY,imagesrc){
    this.bulletX=X;
    this.bulletY=Y;
    this.bulletimage=null;
    this.bulletattach=1;
    this.bulletsizeX=sizeX;
    this.bulletsizeY=sizeY;
//行为
/*
 移动行为
 */
    this.bulletmove=function(){
        this.bulletimage.style.top=this.bulletimage.offsetTop-20+"px";
    }
    this.init=function(){
        this.bulletimage=document.createElement("img");
        this.bulletimage.style.left= this.bulletX+"px";
        this.bulletimage.style.top= this.bulletY+"px";
        this.bulletimage.src=imagesrc;
        mainDiv.appendChild(this.bulletimage);
    }
    this.init();
}
/*
创建单行子弹类
*/
function oddbullet(X,Y,sizeX,sizeY,imagesrc){
   bullet.call(this,X,Y,sizeX,sizeY,imagesrc);
}
//-------子弹类结束

//-----创建爆炸类

function boom(X,Y,sizeX,sizeY,imagesrc){
	this.boomX=X;
	this.boomY=Y;
	this.boomimage=null;
	this.boomsizeX=sizeX;
	this.boomsizeY=sizeY;
	//行为
	this.init=function(){
		this.boomimage=document.createElement("img");
        this.boomimage.style.left= this.boomX+"px";
        this.boomimage.style.top= this.boomY+"px";
        this.boomimage.src=imagesrc;
        mainDiv.appendChild(this.boomimage);
	}
	this.init();
	
}



//-----结束爆炸类

//------创建敌方飞机
/*
创建敌机类
 */
function enemy(hp,a,b,sizeX,sizeY,score,dietime,sudu,imagesrc){
    plan.call(this,hp,random(a,b),0,sizeX,sizeY,score,dietime,sudu,imagesrc);
}
//产生min到max之间的随机数
function random(min,max){
    return Math.floor(min+Math.random()*(max-min));
}
//创建敌方飞机结束


//创建装备类
function jineng(X,Y,sizeX,sizeY,type,imagesrc){
	this.jinengX=X;
	this.jinengY=Y;
	this.jinengsizeX=sizeX;
	this.jinengsizeY=sizeY;
	this.jinengtype=type;
	
	this.jinengimage=null;
	//行为
	
	/*
	 移动行为
	 */
	    this.jinengmove=function(){
	        this.jinengimage.style.top=this.jinengimage.offsetTop+2+"px";
	    }
	this.init=function(){
		this.jinengimage=document.createElement("img");
        this.jinengimage.style.left= this.jinengX+"px";
        this.jinengimage.style.top= this.jinengY+"px";
        this.jinengimage.src=imagesrc;
        mainDiv.appendChild(this.jinengimage);
	}
	this.init();
}
//创建导弹类
function daodan(X,Y,sizeX,sizeY,type,imagesrc){
	this.daodanX=X;
	this.daodanY=Y;
	this.daodansizeX=sizeX;
	this.daodansizeY=sizeY;
	this.daodantype=type;
	
	this.daodanimage=null;
	//行为
	
	/*
	 移动行为
	 */
	    this.daodanmove=function(){
	        this.daodanimage.style.top=this.daodanimage.offsetTop+2+"px";
	    }
	this.init=function(){
		this.daodanimage=document.createElement("img");
        this.daodanimage.style.left= this.daodanX+"px";
        this.daodanimage.style.top= this.daodanY+"px";
        this.daodanimage.style.display="none";
        this.daodanimage.src=imagesrc;
        weapsDiv.appendChild(this.daodanimage);
	}
	this.init();
}


var backgroundPositionY=0;
/*
技能对象数组
 */
var jinengs=[];
/*
敌机对象数组
 */
var enemys=[];
/*
子弹对象数组
 */
var bullets=[];
/*
 * /*
导弹对象数组
 */
var daodans=[];
/*
爆炸对象数组
 */
var booms=[];
var mark=0;
var mark1=0;
var mark2=0;

/*
开始函数
 */
function start(){
	
	 mainDiv.style.backgroundPosition=0+"px"+" "+backgroundPositionY+"px";
	    backgroundPositionY+=0.5;
	   // alert( mainDiv.style.backgroundPosition);
	    //if(mark==5)
	    //clearInterval(set);
	    if(backgroundPositionY==568){
	        backgroundPositionY=0;
	    }
	    mark++;
	    /*
	    创建子弹
	    */
	        if(mark%5==0){
	        	switch(selfplan.weaptype){
	        	
	        	case 0:
	        		var wpone=new oddbullet(parseInt(selfplan.imagenode.style.left)+31,parseInt(selfplan.imagenode.style.top)-30,6,14,"image/zidan.png");
		        	wpone.bulletattach=1;
		        	 bullets.push(wpone);
		        	// daodans.push(new daodan(weapsDiv.style.left+5,weaps.style.top,25,50,1,"image/daodan1.png"));
	        		// bullets.push(new oddbullet(parseInt(selfplan.imagenode.style.left)+31,parseInt(selfplan.imagenode.style.top)-10,"image/wp1.png"));
	        		break;
	        	case 1:
	        		selfplan.imagenode.src="image/own2.png";
	        		var wpone=new oddbullet(parseInt(selfplan.imagenode.style.left)+10,parseInt(selfplan.imagenode.style.top)-130,42,137,"image/wp2.png");
		        	wpone.bulletattach=2;
		        	 bullets.push(wpone);
	        		break;
	        	case 2:
	        		selfplan.imagenode.src="image/own3.png";
	        		var wpone=new oddbullet(parseInt(selfplan.imagenode.style.left)+10,parseInt(selfplan.imagenode.style.top)-70,45,46,"image/wp1.png");
		        	wpone.bulletattach=2;
		        	 bullets.push(wpone);
	        		break;
	        	case 3:
	        		selfplan.imagenode.src="image/own4.png";
	        		var wpone=new oddbullet(parseInt(selfplan.imagenode.style.left)+10,parseInt(selfplan.imagenode.style.top)-130,53,121,"image/wp3.png");
		        	wpone.bulletattach=2;
		        	 bullets.push(wpone);
	        		break;
	        	case 4:
	        		selfplan.imagenode.src="image/own5.png";
	        		
	        		break;
	        	}
	               
	        }
	        /*
	        移动子弹
	        */
	            var bulletslen=bullets.length;
	            for(var i=0;i<bulletslen;i++){
	                bullets[i].bulletmove();
	        /*
	        如果子弹超出边界,删除子弹
	        */
	                if(bullets[i].bulletimage.offsetTop<0){
	                    mainDiv.removeChild(bullets[i].bulletimage);
	                    bullets.splice(i,1);
	                    bulletslen--;
	                }
	            }
	            /*
    	        移动技能
    	        */
    	            var jinengslen=jinengs.length;
    	            for(var i=0;i<jinengslen;i++){
    	            	jinengs[i].jinengmove();
    	        /*
    	        如果技能超出边界,删除技能
    	        */
    	                if(jinengs[i].jinengimage.offsetTop<0){
    	                    mainDiv.removeChild(jinengs[i].jinengimage);
    	                    jinengs.splice(i,1);
    	                    jinengslen--;
    	                }
    	            }
	            
	            /*
	            创建敌方飞机
	             */

	            if(mark==20){
	                mark1++;
	                mark2++;
	                //中飞机
	                if(mark1%5==0){
	                    enemys.push(new enemy(6,25,274,46,60,5000,360,random(1,3),"image/zhongfeiji.png"));
	                }
	                //中飞机
	                if(mark1%10==0){
	                    enemys.push(new enemy(6,25,274,90,81,3000,460,random(1,3),"image/xiaozhong.png"));
	                }
	                //大飞机
	                if(mark1==20){
	                	
	                    enemys.push(new enemy(12,37,160,110,164,30000,540,1,"image/dafeiji.png"));
	                    mark1=0;
	                    
	                }
	                //小飞机
	                else{
	                    enemys.push(new enemy(1,19,286,34,24,1000,360,random(1,4),"image/xiaofeiji.png"));
	                }
	                if(mark2==30){
	                	switch(random(1,4)){
	                	case 1:
	                		jinengs.push(new jineng(random(120,286),random(130,286),34,34,1,"image/zb1.png"));
	                		break;
	                	case 2:
	                		jinengs.push(new jineng(random(120,486),random(130,286),34,34,2,"image/zb2.png"));
	                		break;
	                	case 3:
	                		jinengs.push(new jineng(random(120,286),random(130,286),34,34,3,"image/zb3.png"));
	                		break;
	                	case 4:
	                		jinengs.push(new jineng(random(120,286),random(130,286),34,34,4,"image/zb4.png"));
	                		break;
	                	}
	                  
	                	mark2=0;
	                }
	                mark=0;
	            }
	            /*
	            移动敌方飞机
	             */
	                var enemyslen=enemys.length;
	                for(var i=0;i<enemyslen;i++){
	                    if(enemys[i].planisdie!=true){
	                        enemys[i].planmove();
	                    }
	                   // alert(enemys[i].imagenode.offsetWidth);
	                    //if(mark==4)clearInterval(set);
	                    //当敌方飞机大于边界时候清除敌方飞机
	                    if(enemys[i].imagenode.offsetTop>558){
 	                    	mainDiv.removeChild(enemys[i].imagenode);
 	                    	enemys.splice(i,1);
 	                    	enemyslen--;
 	                    }
	                  //当敌机死亡标记为true时，经过一段时间后清除敌机
	                    if(enemys[i].planisdie==true){
	                        enemys[i].plandietimes+=20;
	                        if(enemys[i].plandietimes==enemys[i].plandietime){
	                            mainDiv.removeChild(enemys[i].imagenode);
	                            enemys.splice(i,1);
	                            enemyslen--;
	                        }
	                    }
	                   
	                }
	                
	                /*
	                碰撞判断
	                */
	                //如果飞机加上技能就产生光圈效果
	                for(var m=0;m<jinengslen;m++){
	                	 if(jinengs[m].jinengimage.offsetLeft+jinengs[m].jinengsizeX>=selfplan.imagenode.offsetLeft&&jinengs[m].jinengimage.offsetLeft<=selfplan.imagenode.offsetLeft+selfplan.plansizeX){
                             if(jinengs[m].jinengimage.offsetTop+jinengs[m].jinengsizeY>=selfplan.imagenode.offsetTop+40&&jinengs[m].jinengimage.offsetTop<=selfplan.imagenode.offsetTop-20+selfplan.plansizeY){
                           //根据技能类型显示相应的技能特效
                            	 selfplan.weaptype=jinengs[m].jinengtype;//将当前技能变成自己的武器
                            	 switch(jinengs[m].jinengtype){
                            case 4:
                            	daodans.push(new daodan(weapsDiv.style.left+5,weaps.style.top,25,50,1,"image/daodan1.png"));
            	        		numdaodan.innerHTML=daodans.length;
            	        		daodanlist.style.display="block";
                            	break;
                            }
                            	 var shengji=new boom(parseInt(jinengs[m].jinengimage.style.left)-40,parseInt(jinengs[m].jinengimage.style.top)-10,128,128,"image/sj.png");
                           
                            setTimeout(function(){ mainDiv.removeChild(shengji.boomimage);}, 300); 
                            mainDiv.removeChild(jinengs[m].jinengimage);
                             jinengs.splice(m,1);
                             jinengslen--;
                             break;
                             }
                             
	                	 }
                             
                             }
	               
	                    for(var k=0;k<bulletslen;k++){
	                        for(var j=0;j<enemyslen;j++){
	                        	 for(var m=0;m<=jinengslen;m++){
	                            //判断碰撞本方飞机
	                            if(enemys[j].planisdie==false){
	                                if(enemys[j].imagenode.offsetLeft+enemys[j].plansizeX>=selfplan.imagenode.offsetLeft&&enemys[j].imagenode.offsetLeft<=selfplan.imagenode.offsetLeft+selfplan.plansizeX){
	                                  if(enemys[j].imagenode.offsetTop+enemys[j].plansizeY>=selfplan.imagenode.offsetTop+40&&enemys[j].imagenode.offsetTop<=selfplan.imagenode.offsetTop-20+selfplan.plansizeY){
	                                      //碰撞本方飞机，游戏结束，统计分数
	                                     // selfplan.imagenode.src="image/bffjbx.gif";
	                                	  new boom(parseInt(selfplan.imagenode.style.left),parseInt(selfplan.imagenode.style.top)-10,70,70,"image/ownbz.png");
	                                      enddiv.style.display="block";
	                                      planscore.innerHTML=scores;
	                                      if(document.removeEventListener){
	                                          mainDiv.removeEventListener("mousemove",yidong,true);
	                                          bodyobj.removeEventListener("mousemove",bianjie,true);
	                                      }
	                                      else if(document.detachEvent){
	                                          mainDiv.detachEvent("onmousemove",yidong);
	                                          bodyobj.removeEventListener("mousemove",bianjie,true);
	                                      }
	                                      clearInterval(set);
	                                  }
	                                }
	                                //判断子弹与敌机碰撞
	                                if((bullets[k].bulletimage.offsetLeft+bullets[k].bulletsizeX>enemys[j].imagenode.offsetLeft)&&(bullets[k].bulletimage.offsetLeft<enemys[j].imagenode.offsetLeft+enemys[j].plansizeX)){
	                                    if(bullets[k].bulletimage.offsetTop<=enemys[j].imagenode.offsetTop+enemys[j].plansizeY&&bullets[k].bulletimage.offsetTop+bullets[k].bulletsizeY>=enemys[j].imagenode.offsetTop){
	                                        //敌机血量减子弹攻击力
	                                        enemys[j].planhp=enemys[j].planhp-bullets[k].bulletattach;
	                                        //敌机血量为0，敌机产生爆炸图片，死亡标记为true，计分
	                                        if(enemys[j].planhp<=0){
	                                            scores=scores+enemys[j].planscore;
	                                            scorelabel.innerHTML=scores;
	                                            
	                                          //  enemys[j].imagenode.src=enemys[j].planboomimage;
	                                            switch(enemys[j].imagenode.offsetWidth){
	                                            case 34://小飞机爆炸效果
	                                            	var xiaofeijiboom= new boom(parseInt(enemys[j].imagenode.style.left)-15,parseInt(enemys[j].imagenode.style.top)-10,20,20,"image/ownbz.png");
	                                            	booms.push(xiaofeijiboom);
	                                            	setTimeout(function(){
	                                            		
	                                            		//mainDiv.removeChild(xiaofeijiboom);
	                                            		var boomslen=booms.length;
	                                            		for(var b=0;b<boomslen;b++){
	                                            			mainDiv.removeChild(booms[b].boomimage);
	                                            			booms.splice(b,1);
	                                            			boomslen--;
	                                            		}
	                                            		
	                                            		}, 300); 
	                                            	break;
	                                            case 46://中飞机爆炸效果
	                                            	var zhongfeijiboom= new boom(parseInt(enemys[j].imagenode.style.left)-10,parseInt(enemys[j].imagenode.style.top)-10,40,40,"image/ownbz.png");
	                                            	booms.push(zhongfeijiboom);
	                                            	setTimeout(function(){ 
	                                            		//mainDiv.removeChild(zhongfeijiboom);
	                                            		var boomslen=booms.length;
	                                            		for(var b=0;b<boomslen;b++){
                                            			mainDiv.removeChild(booms[b].boomimage);
                                            			booms.splice(b,1);
                                            			boomslen--;
                                            		}
                                            		;}, 300); 
	                                            	break;
	                                            case 90://小中飞机爆炸效果
	                                            	var xiaozhongfeijiboom= new boom(parseInt(enemys[j].imagenode.style.left)-12,parseInt(enemys[j].imagenode.style.top)-16,40,40,"image/xzfjbz.png");
	                                            	booms.push(xiaozhongfeijiboom);
	                                            	setTimeout(function(){ 
	                                            		//mainDiv.removeChild(zhongfeijiboom);
	                                            		var boomslen=booms.length;
	                                            		for(var b=0;b<boomslen;b++){
                                            			mainDiv.removeChild(booms[b].boomimage);
                                            			booms.splice(b,1);
                                            			boomslen--;
                                            		}
                                            		;}, 300); 
	                                            	break;
	                                            case 110://大飞机爆炸效果
	                                            	var dafeijiboom= new boom(parseInt(enemys[j].imagenode.style.left)-10,parseInt(enemys[j].imagenode.style.top)+10,80,80,"image/dfjbz.png");
	                                            	booms.push(dafeijiboom);
	                                            	setTimeout(function(){ 
	                                            		//mainDiv.removeChild(dafeijiboom);
	                                            		var boomslen=booms.length;
	                                            		for(var b=0;b<boomslen;b++){
                                            			mainDiv.removeChild(booms[b].boomimage);
                                            			booms.splice(b,1);
                                            			boomslen--;
                                            		}
                                            		;}, 300); 
	                                            	break;
	                                            	
	                                            }
	                                            enemys[j].planisdie=true;
	                                        }
	                                        //删除子弹
	                                        mainDiv.removeChild(bullets[k].bulletimage);
	                                            bullets.splice(k,1);
	                                            bulletslen--;
	                                            break;
	                                    }
	                                }
	                            }
	                        }
	                        }
	                    }
	               
	                
}
/*
开始游戏按钮点击事件
 */
var set;
function begin(){

    startdiv.style.display="none";
    mousediv.style.display="block";
    mainDiv.style.display="block";
    selfplan.imagenode.style.display="block";
    scorediv.style.display="block";
    /*
     调用开始函数
     */
   set=setInterval(start,20);
}
//游戏结束后点击继续按钮事件
function jixu(){
    location.reload(true);
}