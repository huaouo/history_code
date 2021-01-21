import Vue from 'vue'
import Router from 'vue-router'

// Containers
const DefaultContainer = () => import('@/containers/DefaultContainer')

// Dashboard
const Dashboard = () => import('@/views/Dashboard')

// Views - Pages
const Page404 = () => import('@/views/pages/Page404')
const Page500 = () => import('@/views/pages/Page500')
const Login = () => import('@/views/pages/Login')

// Shared
const ChangePassword = () => import('@/views/shared/ChangePassword')

// Student
const StudentInfo = () => import('@/views/student/StudentInfo')
const StudentScore = () => import('@/views/student/StudentScore')
const AcademicQuery = () => import('@/views/student/AcademicQuery')
const ClassCourse = () => import('@/views/student/ClassCourse')

// Teacher
const TeacherInfo = () => import('@/views/teacher/TeacherInfo')
const ScoreInput = () => import('@/views/teacher/ScoreInput')
const StudentRank = () => import('@/views/teacher/StudentRank')
const TeacherCourse = () => import('@/views/teacher/TeacherCourse')

// Admin
const StudentMap = () => import('@/views/admin/StudentMap')
const AddPerson = () => import('@/views/admin/AddPerson')
const DeletePerson= () => import('@/views/admin/DeletePerson')

Vue.use(Router)

var router = new Router({
  mode: 'hash', // https://router.vuejs.org/api/#mode
  linkActiveClass: 'open active',
  scrollBehavior: () => ({y: 0}),
  routes: [
    {
      path: '/',
      redirect: '/dashboard',
      name: 'Home',
      component: DefaultContainer,
      children: [
        {
          path: '/studentInfo',
          name: 'StudentInfo',
          component: StudentInfo
        },
        {
          path: '/changePassword',
          name: 'ChangePassword',
          component: ChangePassword
        },
        {
          path: '/teacherInfo',
          name: 'TeacherInfo',
          component: TeacherInfo
        },
        {
          path: '/scoreInput',
          name: 'ScoreInput',
          component: ScoreInput
        },
        {
          path: '/studentRank',
          name: 'StudentRank',
          component: StudentRank
        },
        {
          path: '/teacherCourse',
          name: 'TeacherCourse',
          component: TeacherCourse
        },
        {
          path: '/studentScore',
          name: 'StudentScore',
          component: StudentScore
        },
        {
          path: '/academicQuery',
          name: 'AcademicQuery',
          component: AcademicQuery
        },
        {
          path: '/classCourse',
          name: 'ClassCourse',
          component: ClassCourse
        },
        {
          path: '/studentMap',
          name: 'StudentMap',
          component: StudentMap
        },
        {
          path: '/addPerson',
          name: 'AddPerson',
          component: AddPerson
        },
        {
          path: '/deletePerson',
          name: 'deletePerson',
          component: DeletePerson
        },
        {
          path: 'dashboard',
          name: 'Dashboard',
          component: Dashboard
        }
      ]
    },
    {
      path: '/pages',
      redirect: '/pages/404',
      name: 'Pages',
      component: {
        render(c) {
          return c('router-view')
        }
      },
      children: [
        {
          path: '404',
          name: 'Page404',
          component: Page404
        },
        {
          path: '500',
          name: 'Page500',
          component: Page500
        },
        {
          path: 'login',
          name: 'Login',
          component: Login
        }
      ]
    }
  ]
})

router.beforeEach(function (to, from, next) {
    if (window.loggedStatus !== 'ADMIN'
      && window.loggedStatus !== 'TEACHER'
      && window.loggedStatus !== 'STUDENT'
      && to.path !== '/pages/login') {
      next('/pages/login')
    } else {
      next(true)
    }
})

export default router;
