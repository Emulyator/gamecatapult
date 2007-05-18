register 'io'
register 'table'
register 'string'

function printf(...) io.write(string.format(...)) end

function Account:show()
  printf("Account balance = $%0.02f\n", self:balance())
end

a = Account(100)
b = Account:new(30)

print('a =', a)
print('b =', b)
print('metatable =', getmetatable(a))
print('Account =', Account)
table.foreach(Account, print)

a:show() a:deposit(50.30) a:show() a:withdraw(25.10) a:show()

parent = {}

function parent:rob(amount)
  amount = amount or self:balance()
  self:withdraw(amount)
  return amount
end

getmetatable(Account).__index = parent

a:test1() b:test2()

b = { x = 32, y = 15, z = 15}
print('b.x = ', b.x)
b.w = 15
