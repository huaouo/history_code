(window["webpackJsonp"]=window["webpackJsonp"]||[]).push([[7],{"+eQS":function(t,e,n){"use strict";var o=n("2XmI"),a=n.n(o);a.a},"2XmI":function(t,e,n){},"83eF":function(t,e,n){"use strict";n.r(e);var o=function(){var t=this,e=t.$createElement,n=t._self._c||e;return n("div",{staticClass:"animated fadeIn"},[n("b-card",{attrs:{"no-body":""}},[n("b-card-header",[n("strong",[t._v("信息管理")])]),n("b-card-body",[n("b-row",{staticClass:"line-control"},[n("b-col",[t._v("教师编号: "+t._s(t.teacherId))]),n("b-col",[t._v("姓名: "+t._s(t.teacherName))])],1),n("b-row",{staticClass:"line-control"},[n("b-col",[t._v("性别: "+t._s(t.sex))]),n("b-col",[t._v("年龄: "+t._s(t.age))])],1),n("b-row",{staticClass:"line-control"},[n("b-col",[t._v("职称: "+t._s(t.title))]),n("b-col",[t._v("联系电话: "+t._s(t.phone))])],1),n("b-button",{attrs:{variant:"primary"},on:{click:t.goChangePassword}},[t._v("修改密码")])],1)],1)],1)},a=[],r=n("vDqi"),l={name:"StudentInfo",methods:{goChangePassword:function(){this.$router.push("/changePassword")}},data:function(){return{teacherId:null,teacherName:null,credit:null,sex:null,age:null,title:null,phone:null}},mounted:function(){var t=this;r({method:"get",url:"/controller/getTeacherInfo"}).then(function(e){if("ILLEGAL"!==e.data)for(var n in e.data)e.data.hasOwnProperty(n)&&(t[n]=e.data[n]);else t.$notify({group:"notify",type:"error",title:"错误",text:"非法操作, 请重新登录后再试!"})}).catch(function(e){console.log(e),t.$notify({group:"notify",type:"error",title:"错误",text:"网络错误!"})})}},c=l,s=(n("+eQS"),n("KHd+")),i=Object(s["a"])(c,o,a,!1,null,"0f6cb3eb",null);e["default"]=i.exports}}]);
//# sourceMappingURL=7.cd7e2787.js.map